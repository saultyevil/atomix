#!/bin/bash
cproto lines.c buffer.c main.c menu.c tools.c ui.c photoionization.c atomic_data.c query.c \
       elements.c ions.c levels.c inner.c parse.c > functions.h
cproto log.c > log.h
