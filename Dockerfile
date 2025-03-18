# Latest LTS Ubuntu
ARG BASE_IMAGE=ubuntu:24.04
FROM $BASE_IMAGE

RUN apt-get update
RUN apt-get upgrade -y
RUN sudo apt-get install build-essential cmake
