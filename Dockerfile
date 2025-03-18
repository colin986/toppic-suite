# Latest LTS Ubuntu
ARG BASE_IMAGE=ubuntu:24.04
FROM $BASE_IMAGE

RUN apt-get update
RUN apt-get upgrade -y

RUN apt-get install build-essential cmake

RUN apt-get install libboost-chrono-dev 
RUN apt-get install libboost-filesystem-dev 
RUN apt-get install libboost-iostreams-dev 
RUN apt-get install libboost-program-options-dev 
RUN apt-get install libboost-thread-dev 
RUN apt-get install libxerces-c-dev  
RUN apt-get install zlib1g-dev 

# install Qt5 for GUI
RUN sudo apt-get install qtbase5-dev
