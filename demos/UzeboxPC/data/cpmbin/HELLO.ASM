        org 100h
        lxi  d,msg
        mvi  c,9
        call 5
        mvi c,0
        call 5
msg:   	db    'Hello world!$'

