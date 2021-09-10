# hyundai-kia-stamp

This is me trying to put together the stamp generation in a docker container for convenience. Since the binaries are extracted from android you need to run them on linux so using docker is a rather easy way to make this easier.

All the hard work goes credit to [@neoPix](https://github.com/neoPix)

### Building

```
docker buildx create --name arm-builder
docker buildx use arm-builder
docker buildx inspect --bootstrap
docker buildx build --platform=linux/arm64 -t hacksore/hks:arm . --load
```

### Before you start

This docker is an ARM build, make sure you're running on a compatible machine or in a quemu environment. Once installed, run the following command to make it work with docker.

```
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
```

### Usage

Run this to generate a `Stamp` header

```
docker run hacksore/hks:arm "<hyundai|kia>" "<single|list>" "<input>"
```

Examples:

```
docker run hacksore/hks:arm "hyundai" "single" "014d2225-8495-4735-812d-2616334fd15d:1614438506419"
docker run hacksore/hks:arm "hyundai" "list" "014d2225-8495-4735-812d-2616334fd15d" > list.txt
docker run hacksore/hks:arm "kia" "list" "e7bcd186-a5fd-410d-92cb-6876a42288bd" > list.txt
```