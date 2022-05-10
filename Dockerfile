FROM alpine AS build

COPY . /workspace
WORKDIR /workspace

RUN 'apk add gcc'
RUN 'gcc src/main.c -o aes_whitebox'

FROM alpine AS dist

WORKDIR /workspace
ENV BASE_PATH /workspace
COPY ./lib /workspace/lib
COPY --from=build ./aes_whitebox /workspace/aes_whitebox

ENTRYPOINT bash -e $BASE_PATH/lib/exec.sh $0 $1 $2