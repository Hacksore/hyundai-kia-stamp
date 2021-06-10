FROM --platform=$BUILDPLATFORM arm64v8/maven

COPY . /workspace
WORKDIR /workspace

RUN mvn package
ENV BASE_PATH=/workspace

ENTRYPOINT bash -e $BASE_PATH/lib/exec.sh $0 $1 $2