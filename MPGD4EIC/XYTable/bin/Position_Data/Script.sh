#!bash
__DAQREADY__=`rcdaq_client daq_list_readlist -s`
if [ -z "$__DAQREADY__" ]
then
    source setup_daq.sh
fi

