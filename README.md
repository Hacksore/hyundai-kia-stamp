# hyundai-kia-stamp

This is me trying to put together the stamp generation in a docker container for convenience. Since the binaries are extracted from android you need to run them on linux so using docker is a rather easy way to make this easier.

All the hard work goes credit to [@neoPix](https://github.com/neoPix)


### Usage

Run this to generate a `Stamp` header

```
docker run hacksore/hks "<input>"
```

Example:

```
docker run hacksore/hks "99cfff84-f4e2-4be8-a5ed-e5b755eb6581:1614438506419"
```