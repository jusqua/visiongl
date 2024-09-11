#!/usr/bin/env bash
TARGET=samples

git clone https://github.com/ddantas/visiongl_images.git $TARGET
cd $TARGET
unzip mitosis.zip
cd ..

echo Sample images downloaded to folder "./$TARGET/"
