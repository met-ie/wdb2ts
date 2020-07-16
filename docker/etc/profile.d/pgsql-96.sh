# Set up path for postgresql 9.6

pathmunge () {
    case ":${PATH}:" in
        *:"$1":*)
            ;;
        *)
            if [ "$2" = "after" ] ; then
                PATH=$PATH:$1
            else
                PATH=$1:$PATH
            fi
    esac
}


pathmunge /usr/pgsql-9.6/bin after
export PATH

unset -f pathmunge
