FROM archlinux

RUN mkdir /labworks
WORKDIR /labworks
COPY . /labworks/

RUN yes | pacman -Syu
RUN yes | pacman -S gcc cmake go gtest fmt openssl 
