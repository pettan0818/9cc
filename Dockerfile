FROM ubuntu:18.04

RUN apt-get update

CMD mkdir -p /root/files

ADD ./files /root/files

