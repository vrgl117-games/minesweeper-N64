FROM anacierdem/libdragon

RUN cd /tmp && git clone https://github.com/sezero/mikmod.git && cd mikmod/libmikmod/n64 && make -f Makefile.n64 install

WORKDIR /game
