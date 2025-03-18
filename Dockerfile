# Latest LTS Ubuntu
ARG BASE_IMAGE=ubuntu:24.04
FROM $BASE_IMAGE

RUN apt-get update
RUN apt-get upgrade -y

RUN apt-get install -y build-essential cmake

RUN apt-get install -y libboost-chrono-dev 
RUN apt-get install -y libboost-filesystem-dev 
RUN apt-get install -y libboost-iostreams-dev 
RUN apt-get install -y libboost-program-options-dev 
RUN apt-get install -y libboost-thread-dev 
RUN apt-get install -y libxerces-c-dev  
RUN apt-get install -y zlib1g-dev 

# install Qt5 for GUI
RUN sudo apt-get install -y qtbase5-dev
