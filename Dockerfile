# Latest LTS Ubuntu
ARG BASE_IMAGE=ubuntu:24.04
FROM $BASE_IMAGE

RUN apt install build-essential cmake
