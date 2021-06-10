FROM arm32v7/maven:3-adoptopenjdk-16

COPY . /workspace
WORKDIR /workspace

RUN mvn package
ENV BASE_PATH=/workspace

ENTRYPOINT bash -e $BASE_PATH/lib/exec.sh $0 $1 $2