set -e

OPENCV_PATH="/opt/homebrew/opt/opencv"
BIN_DIR="Users/griffinpendrick/my-lane/bin"
SRC_DIR="Users/griffinpendrick/my-lane/src"

mkdir -p "$BIN_DIR"
pushd "$BIN_DIR" > /dev/null

CompilerFlags="-O0 -g -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter"
IncludePath="-I${OPENCV_PATH}/include/opencv4"
LinkerFlags="-L${OPENCV_PATH}/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio"

clang++ $CompilerFlags $IncludePath "$SRC_DIR/main.cpp" $LinkerFlags -o mylane

popd > /dev/null
