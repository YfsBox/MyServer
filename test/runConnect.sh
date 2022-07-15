#!/bin/zsh

set -x

curl_test() {
    time=${1}
    url=${2}
    while true
    do
        for i in $(seq 1 ${time})
        do
        if (curl ${url} > /dev/null 2>&1) &
        then
            echo OK
        else
            echo
        fi
        done
        sleep 3
    done
}

curl_test ${1} ${2}

