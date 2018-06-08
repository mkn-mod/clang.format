# clang.format

** clang-format maiken module **

Link phase module

## Prerequisites
  [maiken](https://github.com/Dekken/maiken)

## Usage

```yaml
mod:
- name: clang.format
  link:
    args:  $str       #[optional, default="", additional values to pass to clang-format]
    style: file       #[optional, default=file, can be google/mozilla]
    types: cpp:cxx:cc #[optional, default=cpp:cxx:cc:c:h:hpp]
    paths: inc src    #[optional, inherits sources but not include paths]
```

## Building

  Windows cl:

    mkn clean build -tSa -EHsc -d


  *nix gcc:

    mkn clean build -tSa "-O2 -fPIC" -d -l "-pthread -ldl"


## Testing

  Windows cl:

    mkn clean build -tSa -EHsc -dp test run


  *nix gcc:

    mkn clean build -tSa "-O2 -fPIC" -dp test -l "-pthread -ldl" run

## WARNINGS

This can rearrange your header includes which may break your build!



## Environment Variables

    Key             CLANG_HOME
    Type            string
    Default         ""
    Description     If set - looks for adds $CLANG_HOME/bin to PATH
