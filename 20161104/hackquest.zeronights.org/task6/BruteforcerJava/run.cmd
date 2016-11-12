@echo off
set path=C:\Program Files\Java\jdk1.8.0_111\bin\;%path%
del classes\TestClass.class >nul 2>&1

javac classes\TestClass.java
jar cfm test.jar MANIFEST.MF ./
java -jar test.jar %*