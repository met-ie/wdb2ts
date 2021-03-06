/*
    wdb - weather and water data storage

    Copyright (C) 2007 met.no

    Contact information:
    Norwegian Meteorological Institute
    Box 43 Blindern
    0313 OSLO
    NORWAY
    E-mail: wdb@met.no
  
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
    MA  02110-1301, USA
*/
//#include <unistd.h>
//#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <list>
#include <boost/thread/tss.hpp>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <replace.h>
#include <RequestHandler.h>
#include <IAbortHandlerManager.h>
#include <App.h>


using namespace std;

namespace {

struct AbortHandlerHelper {
	webfw::Request *req;
	webfw::IAbortHandlerManager *mgr;
	std::list<unsigned long> idList;
	
	AbortHandlerHelper( webfw::Request *req_, webfw::IAbortHandlerManager *mgr_ )
		:  req( req_ ), mgr( mgr_ ) {}
	
	~AbortHandlerHelper() 
		{         
			if( ! mgr )
				return;
			
			mgr->removeAllAbortHandler( idList );
		}
	
	unsigned long  registerAbortHandler( webfw::IAbortHandler *handler )
		{
			if( ! mgr )
				return 0;
			
			unsigned long ret = mgr->registerAbortHandler( req, handler );

			if( ret > 0 )
				idList.push_back( ret );
			else
				delete handler;
		
			return ret;
		}
	
	void removeAbortHandler( unsigned long handlerId ) 
		{
			if( ! mgr )
				return;
			
			for( std::list<unsigned long>::iterator it=idList.begin(); it != idList.end(); ++it ) {
				if( *it == handlerId ) {
					idList.erase( it );
					mgr->removeAbortHandler( handlerId );
					return;
				}
			}
		}
};


struct ThreadData {
	AbortHandlerHelper *abortHandlerHelper;
	std::string        logprefix;
	const webfw::RequestHandler *requestHandler;
	webfw::App *app;

	ThreadData() 
		: abortHandlerHelper( 0 ), requestHandler( 0 ), app ( 0 )
		{
			//cerr << "*** ThreadData: default CTOR." << endl;

		}
	
	ThreadData( AbortHandlerHelper *abortHandler, const webfw::RequestHandler *reqHandler, webfw::App *app_ )
		: abortHandlerHelper( abortHandler ), requestHandler( reqHandler ), app( app_ )
		{
			logprefix = reqHandler->getLogprefix();

			if( logprefix.empty() ) {
				logprefix = abortHandler->req->urlPath();
				miutil::replaceString( logprefix, " ", "_");
				miutil::replaceString( logprefix, "/", ".");
			
				//Remove . and _ from the start, if any.
				while( ! logprefix.empty() && ( logprefix[0] == '.' || logprefix[0] == '_') )
					logprefix.erase( 0, 1 );
			
				//Remove . and _ from the end, if any.
				while( ! logprefix.empty() && ( logprefix[ logprefix.length()-1 ] == '.' || logprefix[ logprefix.length()-1 ] == '_') )
					logprefix.erase(  logprefix.length()-1 );
			}

			//cerr << "*** ThreadData: CTOR. logprefix: " <<  logprefix << endl;
		}

	~ThreadData() {
		//cerr << "*** ThreadData: DTOR. logprefix: " <<  logprefix << endl;

		//if( ! abortHandlerHelper && requestHandler && ! app )
		//	cerr << "*** ThreadData: DTOR. Only requestHandler." <<  endl;

		if( abortHandlerHelper )
			delete abortHandlerHelper;
	}
};

boost::thread_specific_ptr<ThreadData> threadDataTSS;
boost::mutex     setupLoggerMutex;

void 
prepareThreadData( webfw::App *app, webfw::RequestHandler *reqHandler, webfw::Request *req, webfw::IAbortHandlerManager *mgr, const std::string &method )
{
	AbortHandlerHelper *abortHelper = 0;
	ThreadData *threadData = threadDataTSS.get();
	
	if( threadData ) {
		cerr << "WARNING WARNING: RequestHandler::doGet: Serious bug an ThreadData allready exist." << endl;
		//We have a serious bug that will pro	ably make the program crach sooner or later.
		threadDataTSS.release(); 
	}
	
	try{
		abortHelper = new AbortHandlerHelper( req, mgr );
		threadData = new ThreadData( abortHelper, reqHandler, app );
		threadDataTSS.reset( threadData );
	}
	catch( ... ) {
		cerr << "WARNING WARNING: RequestHandler::" << method << " Failed to alocate an ThreadData." << endl;
		if( abortHelper )
			delete abortHelper;
		abortHelper = 0;
		threadData = 0;
	}

}

void
cleanupThreadData( const std::string &method )
{
	ThreadData *threadData = threadDataTSS.release();

	if( threadData )
		delete threadData;
}

void
setupLogger( const std::string &name_,
		     const webfw::RequestHandler *reqHandler )
{

	boost::mutex::scoped_lock lock( setupLoggerMutex );
	string name( name_ );
	string filename;
	string filenameExtra;
	string category;
	std::string logdir;
	std::string prefix;
	std::string module;

	if( reqHandler ) {
		logdir = reqHandler->getLogDir();
		prefix = reqHandler->getLogprefix();
		module = reqHandler->getModuleName();
	}

	if( !name.empty() && name[0]=='+' ) {
	   name.erase(0, 1);
	   filenameExtra = "_" + name;
	}

	if( prefix.empty() )
		category = name;
	else
		category = prefix + "." + name;
	
	if( log4cpp::Category::exists( category ) )
		return;

	if( ! module.empty() )
		filename = module +"-";

	std::string tmpprefix = prefix;
	miutil::replaceString( tmpprefix, ".", "_");

	if( tmpprefix.empty() )
		filename.erase();
	else if( !logdir.empty() )
		filename = logdir + "/" + filename + tmpprefix+filenameExtra+".log";
	else
		filename.erase();
	
	log4cpp::Appender *appender=0;
	
	if( reqHandler  && !filename.empty()) {
	   //If the 'name' contains a + in front we create a own file
	   //for this category. Otherwise we use the category given with
	   //prefix as a dummy category to hold an appender. All subcategories
	   //use the same appender as this.

	   if( !filenameExtra.empty() )
	      prefix = category;

		if( ! log4cpp::Category::exists( prefix ) ) {
			cerr << "Loggsystem: No appender exist for '" << prefix << "' filename: '" << filename << "'"<< endl;
			errno = 0;
			appender = new log4cpp::RollingFileAppender( prefix, filename, 10 * 1024 * 1024, 10, true, 00640 );

			if( errno != 0 ) {
			    char buf[256];
			    strerror_r( errno, buf, 256 );
			    buf[255]='\0';
			    cerr << "ERROR: Loggsystem: errno: " << errno << " what: " << buf << "." << endl;

//				cerr << " Loggsystem: **** errno: " << savedErr << " '" << buf << "'. uid: " << getuid()
//			         << " euid: " << geteuid() << " gid: " << getgid() << " egid: " << getegid() << endl;


				delete appender;
				appender=0;
			} else {
				ostringstream ost;
				log4cpp::PatternLayout *layout = new log4cpp::PatternLayout();

				ost <<"%d{%d-%m-%Y %H:%M:%S,%l} %p %c : %m%n";
				layout->setConversionPattern( ost.str() );
				appender->setLayout( layout );
				log4cpp::Category &logger = log4cpp::Category::getInstance( prefix );
				logger.setAdditivity( false );
				logger.addAppender( appender );
				logger.setPriority( log4cpp::Priority::DEBUG );
			}
		} else {
			log4cpp::Category &logger = log4cpp::Category::getInstance( prefix );
			appender = logger.getAppender();

		}
	} else {
		cerr << "Add logger. Logging to default logfile."  << endl;

		if( ! log4cpp::Category::exists( "__WDB2TS_DEFAULT__" ) ) {
			ostringstream ost;
			log4cpp::PatternLayout *layout = new log4cpp::PatternLayout();

			ost <<"%d{%d-%m-%Y %H:%M:%S,%l} %p %c : %m%n";
			layout->setConversionPattern( ost.str() );
			appender = new log4cpp::OstreamAppender( "TheLogger", & std::cerr );
			appender->setLayout( layout );
			log4cpp::Category &logger = log4cpp::Category::getInstance( prefix );
			logger.setAdditivity( false );
			logger.addAppender( appender );
			logger.setPriority( log4cpp::Priority::DEBUG );
		} else {
			log4cpp::Category &logger = log4cpp::Category::getInstance( "__WDB2TS_DEFAULT__" );
			appender = logger.getAppender();
		}
	}
	
	log4cpp::Priority::Value logLevel;
	
	if( reqHandler ) {
		switch( reqHandler->getLogLevel( name ) ) {
			case 9: logLevel = log4cpp::Priority::NOTSET; break;
			case 8: logLevel = log4cpp::Priority::DEBUG; break;
			case 7: logLevel = log4cpp::Priority::INFO; break;
			case 6: logLevel = log4cpp::Priority::NOTICE; break;
			case 5: logLevel = log4cpp::Priority::WARN; break;
			case 4: logLevel = log4cpp::Priority::ERROR; break;
			case 3: logLevel = log4cpp::Priority::CRIT; break;
			case 2: logLevel = log4cpp::Priority::ALERT; break;
			case 1: logLevel = log4cpp::Priority::FATAL; break;
			case 0: logLevel = log4cpp::Priority::EMERG; break;
			default:
				logLevel = log4cpp::Priority::NOTSET;
		}
	} else {
		logLevel = log4cpp::Priority::NOTSET;
	}
	
	if( ! appender ) {
		cerr << "webfw::setupLogger: Failed to create log appender for: " << category << endl;
		return;
	}

	cerr << "Add logger category: " << category <<". Pid: " << getpid() << endl;
	log4cpp::Category &logger = log4cpp::Category::getInstance( category );
	logger.setAdditivity( false );
	logger.addAppender( *appender );
	logger.setPriority( logLevel );
}



}


void
webfw:: 
RequestHandler::
setVersion( int &major, int &minor )
{
   majorVersion_ = major;
   minorVersion_ = minor; 
}
   
webfw::   
RequestHandler::
RequestHandler()
{
}

webfw::
RequestHandler::
~RequestHandler()
{
}
   
   
void 
webfw::
RequestHandler::
version( int &major, int &minor )const
{
   major = majorVersion_;
   minor = minorVersion_;
}

bool 
webfw::
RequestHandler::
isVersion( int major, int minor ) const 
{ 
   return majorVersion_==major &&
          minorVersion_==minor;    
}

void 
webfw::
RequestHandler::
setPaths( const std::string &confpath,
		  const std::string &logpath,
		  const std::string &tmppath )
{
	confDir = confpath;
	logDir = logpath;
	tmpDir = tmppath;
}

void
webfw::
RequestHandler::
doGet(Request &req, Response &response, Logger &logger, App *app )
{
	prepareThreadData( app, this, &req, app->abortHandler(), "doGet" );

	try {
		get( req, response, logger );
		cleanupThreadData( "doGet" );
	}
	catch( ... ) {
		//We must have a chanse to  remove the abortHandlerHelper and remove all AbortHandlers 
		//before we return.
		cleanupThreadData( "doGet" );
		throw;
	}
}
	

void 
webfw::
RequestHandler::
doPost( Request &req, Response &response, Logger &logger, App *app )
{
	prepareThreadData( app, this, &req, app->abortHandler(), "doPost" );

	try {
		post( req, response, logger );
		cleanupThreadData( "doPost" );
	}
	catch( ... ) {
		//We must have a chanse to  remove the abortHandlerHelper and remove all AbortHandlers 
		//before we return.
		cleanupThreadData( "doPost" );
		throw;
	}
}

void
webfw::
RequestHandler::
doPut( Request &req, Response &response, Logger &logger, App *app )
{
	prepareThreadData( app, this, &req, app->abortHandler(), "doPut" );

	try {
		put( req, response, logger );
		cleanupThreadData( "doPut" );
	}
	catch( ... ) {
		//We must have a chanse to  remove the abortHandlerHelper and remove all AbortHandlers 
		//before we return.
		cleanupThreadData( "doPut" );
		throw;
	}
}

void 
webfw::
RequestHandler::
doDel( Request &req, Response &response, Logger &logger, App *app )
{
	prepareThreadData( app, this, &req, app->abortHandler(), "doDel" );

	try {
		del( req, response, logger );
		cleanupThreadData( "doDel" );
	}
	catch( ... ) {
		//We must have a chanse to  remove the abortHandlerHelper and remove all AbortHandlers 
		//before we return.
		cleanupThreadData( "doDel" );
		throw;
	}
}

unsigned long
webfw::
RequestHandler::
registerAbortHandler( IAbortHandler *handler )
{
	ThreadData *threadData = threadDataTSS.get();
		
	if( ! threadData || ! threadData->abortHandlerHelper ) {
		cerr << "WARNING WARNING: RequestHandler::registerAbortHandler: Strange no abortHandlerHelper exist." << endl;
		return 0;	
	}
	
	return threadData->abortHandlerHelper->registerAbortHandler( handler );
	
}

void 
webfw::
RequestHandler::
removeAbortHandler( unsigned long handlerId )
{
	ThreadData *threadData = threadDataTSS.get();
		
	if( ! threadData || ! threadData->abortHandlerHelper ) {
		cerr << "WARNING WARNING: RequestHandler::removeAbortHandler: Strange no abortHandlerHelper exist." << endl;
		return;	
	}
	
	threadData->abortHandlerHelper->removeAbortHandler( handlerId );
}


void 
webfw::
RequestHandler::
get( Request &req, Response &response, Logger &logger )
{
   response.status( Response::GET_NOT_SUPPORTED );
}

void 
webfw::
RequestHandler::
post( Request &req, Response &response, Logger &logger )
{
   response.status( Response::POST_NOT_SUPPORTED );
}

void 
webfw::
RequestHandler::
put( Request &req, Response &response, Logger &logger )
{
   response.status( Response::PUT_NOT_SUPPORTED );
}

void 
webfw::
RequestHandler::
del( Request &req, Response &response, Logger &logger )
{
   response.status( Response::DELETE_NOT_SUPPORTED );
}

void
webfw::
RequestHandler::
setLogLevels( const std::map<std::string, int > &loglevels )
{
	logLevels = loglevels;
}


int
webfw::
RequestHandler::
getLogLevel( const std::string &name ) const
{
	map<string, int >::const_iterator it = logLevels.find( name );
	
	if( it == logLevels.end() ) {
		it = logLevels.find("default");
		
		if( it == logLevels.end() ) {
			//return 6; //NOTICE
			return 8; //DEBUG
		}
	}
	
	return it->second;
}


const webfw::RequestHandler*
webfw::
RequestHandler::
getRequestHandler()
{
	ThreadData *threadData = threadDataTSS.get();

	if( ! threadData )
		return 0;

	return threadData->requestHandler;
}

log4cpp::Category&
webfw::
RequestHandler::
getLogger( const std::string &name, const RequestHandler *reqHandler )
{
	std::string category( name );
	ThreadData *threadData = threadDataTSS.get();
	const RequestHandler *handler = reqHandler;

	if( !category.empty() && category[0]=='+')
	   category.erase( 0, 1 );

	if( threadData )
		handler = threadData->requestHandler;
	else if( handler )	{
		threadData = new ThreadData( 0, reqHandler, 0 );
		threadDataTSS.reset( threadData );
	}

	if( handler ) {
		category.insert(0, threadData->logprefix + ".");

		if( ! log4cpp::Category::exists( category ) )
			setupLogger( name, handler );
			
		return log4cpp::Category::getInstance( category );
	} else {
		if( ! log4cpp::Category::exists( category ) )
			setupLogger( name, 0 );

		return log4cpp::Category::getInstance( category );
	}
}

