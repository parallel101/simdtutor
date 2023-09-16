FROM intel/oneapi-basekit
RUN DEBIAN_FRONTEND=noninteractive apt update
RUN apt install -y libnss3-dev
RUN apt install -y libatk1.0-dev
RUN apt install -y libatk-bridge2.0-dev
RUN apt install -y libcups2-dev
RUN apt install -y libdrm-dev
RUN apt install -y libgtk-3-dev
RUN apt install -y libasound2-dev
RUN apt install -y x11-apps x11-xserver-utils
RUN apt install -y vim cmake make
RUN mkdir -p /root/workspace
WORKDIR /root/workspace
