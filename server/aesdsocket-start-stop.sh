#!/bin/sh

DAEMON="aesdsocket"

if [ "$1" = "start" ]; then
    aesdsocket -d
    exit $?
fi

if [ "$1" = "stop" ]; then
    PID=$(pgrep $DAEMON)
    if [ -z "$PID" ]; then
        echo "Processo '$DAEMON' não encontrado."
        exit 1
    fi

    for P in $PID;
    do
        kill -s 15 $P

        if [ $? -eq 0 ]; then
            echo "Sinal SIGTERM enviado para o processo '$DAEMON' (PID: $PID)."
	    exit 0
        fi
    done
    exit $?
fi

exit 1
