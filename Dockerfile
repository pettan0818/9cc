FROM alpine:3.6

RUN apk update
RUN apk add zsh vim git grep less

RUN mkdir -p /root/files

RUN git clone http://github.com/pettan0818/9cc
