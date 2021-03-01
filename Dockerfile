FROM maven

COPY . /workspace
WORKDIR /workspace

RUN mvn clean package
ENV BASE_PATH=/workspace

ENTRYPOINT bash -e $BASE_PATH/lib/exec.sh $0 $1 $2