
if [ "$1" = "--clean" ]; then
  echo Cleaning...
  rm -Rf ./target
fi

export X_CMAKE_UNCRUSTIFY_ENABLED=1
export X_CMAKE_OPEN_SSL=true

echo Creating Target Directory...
mkdir ./target

cd ./target

cmake .. && make && ctest -C Release --output-on-failure

if [ "$1" = "--memcheck" ]; then
  # memory check
  ctest -T memcheck || true
fi

cd -
