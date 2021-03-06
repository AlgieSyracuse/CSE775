/////////////////////////////////////////////////////////////////////////
// FileSystemTest.cpp - Unit test cases for the FileSystem namespace   //
// ver 1.1                                                             //
// Jim Fawcett, Summer Projects, 2012                                  //
/////////////////////////////////////////////////////////////////////////
/*
 * ver 1.1 : 23 Feb 13
 * - added more tests for Path::getExt(...), e.g., cases for
 *   *.cpp, ../noext, ..\\someFile.ext
 */

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "FileSystem.h"
#include "UnitTest.h"

using namespace FileSystem;

class TestFile : public ITest<TestFile>
{
public:
  TestFile() : testFilePath("../FileSystemDemo/FileSystem.cpp")
  {
    std::cout << "\n\n  Testing File class";
  }
  bool testFileName()
  {
    File testFile(testFilePath);
    std::string name = Path::getName(testFile.name());
    std::string check = Path::getName(testFilePath);
    return checkResult(name == check, "testFileName");
  }

  bool testFileGetAndPutLine()
  {
    std::string testline = "a test line";
    File outTestFile("tempTestFile.txt");
    outTestFile.open(File::out);
    if(outTestFile.isGood())
    {
      outTestFile.putLine(testline);
      outTestFile.putLine(testline);
    }
    std::string line1, line2;
    File inTestFile("tempTestFile.txt");
    inTestFile.open(File::in);
    if(inTestFile.isGood())
    {
      line1 = inTestFile.getLine();
      line2 = inTestFile.getLine();
    }
    return checkResult(
      line1 == testline && line2 == testline, 
      "testFileGetAndPutLine"
    );
  }

  bool testFileGetAndPutBlock()
  {
    Byte blockArray[] = { 'a', 'b', 'c', 'd' };
    Block block(&blockArray[0], &blockArray[4]);
    File outTestFile("tempTestFile.bin");
    outTestFile.open(File::out, File::binary);
    if(outTestFile.isGood())
    {
      outTestFile.putBlock(block);
      outTestFile.putBlock(block);
      outTestFile.close();
    }
    Block block1, block2;
    File inTestFile("tempTestFile.bin");
    inTestFile.open(File::in, File::binary);
    if(inTestFile.isGood())
    {
      block1 = inTestFile.getBlock(block.size());
      block2 = inTestFile.getBlock(block.size());
    }
    return checkResult(
      block1 == block && block2 == block, 
      "testFileGetAndPutBlock"
    );
  }

  bool testFileExists()
  {
    bool t1, t2;
    std::string testFileSpec = "test.txt";
    t1 = File::exists(testFileSpec);
    testFileSpec = "foobar";
    t2 = !File::exists(testFileSpec);
    return checkResult(t1 & t2, "testFileExists");
  }

  bool testFileCopyAndRemove()
  {
    bool t1, t2, t3;
    std::string testFileSpec = "test.txt";
    std::string dst = "dsttest.txt";
    if(File::exists(testFileSpec))
    {
      t1 = File::copy(testFileSpec,dst,false);
      t2 = File::exists(dst);
      t3 = File::remove(dst);
      return checkResult(t1 && t2 && t3, "testFileCopyAndRemove");
    }
    else
    {
      std::cout << "\n  " << testFileSpec << " does not exist";
      return failed("testFileCopyAndRemove");
    }
  }

  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestFile::testFileName);
    t2 = doTest(&TestFile::testFileGetAndPutLine);
    t3 = doTest(&TestFile::testFileGetAndPutBlock);
    t4 = doTest(&TestFile::testFileExists);
    t5 = doTest(&TestFile::testFileCopyAndRemove);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
    const std::string testFilePath;
};

class TestFileInfo : public ITest<TestFileInfo>
{
public:
  TestFileInfo() : testFilePath("FileSystem.cpp")
  {
    std::cout << "\n\n  Testing FileInfo class";
  }
  bool testFileInfoConstruction()
  {
    FileInfo fi(testFilePath);
    return checkResult(fi.good(), "testFileInfoConstruction");
  }
  bool testFileInfoName()
  {
    FileInfo fi(testFilePath);
    std::string name = fi.name();
    std::string check = Path::getName(testFilePath);

    //throw std::exception("testFileInfoName");
    
    return checkResult(name == check, "testFileInfoName");
  }
  bool testFileInfoDate()
  {
    bool t1, t2, t3;
    FileInfo fi(testFilePath);
    std::string date = fi.date(FileInfo::fullformat);
    t1 = date.find("/") < date.size() && date.find(":") < date.size();
    date = fi.date(FileInfo::dateformat);
    t2 = date.find("/") < date.size() && date.find(":") >= date.size();
    date = fi.date(FileInfo::timeformat);
    t3 = date.find("/") >= date.size() && date.find(":") < date.size();
    return checkResult(t1 && t2 && t3, "testFileInfoDate");
  }
  bool testFileInfoSize()
  {
    bool t1, t2;
    File testSize("testsize");
    testSize.open(File::out, File::binary);
    FileInfo fi("testsize");
    size_t size = fi.size();
    t1 = size == 0;
    Byte bytes[] = { 'a', 'b', 'c', 'd' };
    Block blk(&bytes[0], &bytes[4]);
    testSize.putBlock(blk);
    testSize.close();
    FileInfo fi2("testsize");
    size = fi2.size();
    t2 = size == 4;
    return checkResult(t1 && t2, "testFileInfoSize");
  }

  bool testFileInfoNameCompare()
  {
    bool t1, t2, t3, t4, t5, t6, t7, t8;
    t1 = t2 = t3 = t4 = t5 = t6 = t7 = t8 = false;
    File testA("testA");
    testA.open(File::out);
    if(testA.isGood())
      testA.putLine("a line of text");
    testA.close();
    //usleep(1000000);  // make file times different
    File testB("testB");
    testB.open(File::out);
    if(testB.isGood())
      testB.putLine("text");
    testB.close();
    FileInfo fiA("testA");
    FileInfo fiB("testB");
    t1 = fiA < fiB;
    t2 = !(fiA == fiB);
    t3 = !(fiA > fiB);
    t4 = fiA.earlier(fiB);
    t5 = !fiA.later(fiB);
    size_t sa = fiA.size();
    size_t sb = fiB.size();
    t6 = fiA.larger(fiB);
    t7 = !fiA.smaller(fiB);
    //std::cout << "\n  t1 = " << t1;
    //std::cout << "\n  t2 = " << t2;
    //std::cout << "\n  t3 = " << t3;
    //std::cout << "\n  t4 = " << t4;
    //std::cout << "\n  t5 = " << t5;
    //std::cout << "\n  t6 = " << t6;
    //std::cout << "\n  t7 = " << t7;
    return checkResult(
      t1 && t2 && t3 && t4 && t5 && t6 && t7, 
      "testFileInfoNameCompare"
    );
  }
  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestFileInfo::testFileInfoConstruction);
    t2 = doTest(&TestFileInfo::testFileInfoName);
    t3 = doTest(&TestFileInfo::testFileInfoDate);
    t4 = doTest(&TestFileInfo::testFileInfoSize);
    t5 = doTest(&TestFileInfo::testFileInfoNameCompare);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
  const std::string testFilePath;
};

class TestPath : public ITest<TestPath>
{
public:
  TestPath() : testFilePath("../FileSystemDemo/FileSystem.cpp")
  {
    std::cout << "\n\n  Testing Path class";
  }
  bool TestPathGetFullFileSpec()
  {
    std::string currPath = Directory::getCurrentDirectory();
    std::string fileSpec = Path::getFullFileSpec(".");
    return checkResult(currPath == fileSpec, "TestPathGetFullFileSpec");
  }
  bool TestPathGetPath()
  {
    bool t1, t2;
    std::string path = Path::getPath("../foobar/someFile.txt");
    std::string expected = "../foobar/";
    t1 = path == expected;
    path = Path::getPath("../foobar/someFile.txt");
    expected = "../foobar/";
    t2 = path == expected;
    return checkResult(t1 && t2, "TestPathGetPath");
  }
  bool TestPathGetName()
  {
    bool t1, t2, t3;
    std::string name = Path::getName("../foobar/someFile.txt");
    std::string expected = "someFile.txt";
    t1 = name == expected;
    name = Path::getName("../foobar/someFile.txt", false);
    expected = "someFile";
    t2 = name == expected;
    name = Path::getName("../foobar/someFile");
    expected = "someFile";
    t3 = name == expected;
    return checkResult(t1 && t2 && t3, "TestPathGetName");
  }
  bool TestPathGetExt()
  {
    bool t1, t2, t3, t4, t5;
    std::string ext = Path::getExt("../foobar/someFile.txt");
    std::string expected = "txt";
    t1 = ext == expected;
    ext = Path::getExt("../foobar/someFile.txt");
    expected = "txt";
    t2 = ext == expected;
    ext = Path::getExt("../foobar/someFile");
    expected = "";
    t3 = ext == expected;
    ext = Path::getExt("../someDir/someFile.cpp");
    expected = "cpp";
    t4 = ext == expected;
    ext = Path::getExt("*.cpp");
    expected = "cpp";
    t5 = ext == expected;
    return checkResult(t1 && t2 && t3 && t4 && t5, "TestPathGetExt");
  }
  bool TestPathFileSpec()
  {
    bool t1, t2, t3;
    std::string path = "./foo";
    std::string name = "bar.txt";
    std::string expected = "./foo/bar.txt";
    std::string fileSpec = Path::fileSpec(path,name);
    t1 = fileSpec == expected;
    path = "../foo";
    name = "bar";
    expected = "../foo/bar";
    fileSpec = Path::fileSpec(path,name);
    t2 = fileSpec == expected;
    path = "C:/foo";
    name = "bar.cpp";
    expected = "C:/foo/bar.cpp";
    fileSpec = Path::fileSpec(path,name);
    t3 = fileSpec == expected;
    return checkResult(t1 && t2 && t3, "TestPathFileSpec");
  }
  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestPath::TestPathGetFullFileSpec);
    t2 = doTest(&TestPath::TestPathGetPath);
    t3 = doTest(&TestPath::TestPathGetName);
    t4 = doTest(&TestPath::TestPathGetExt);
    t5 = doTest(&TestPath::TestPathFileSpec);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
  const std::string testFilePath;
};

class TestDirectory : public ITest<TestDirectory>
{
public:
  TestDirectory() : testPath_("./temp")
  {
    if(!Directory::exists(testPath_))
      Directory::create(testPath_);
    std::cout << "\n\n  Testing Directory class";
  }
  bool TestDirectoryCreateRemoveExists()
  {
    bool t1, t2, t3;
    std::string path = testPath_ + "/TestDir";
    t1 = Directory::create(path) || Directory::exists(path);
    //::Sleep(100);
    t2 = Directory::exists(path);
    //::Sleep(100);
    t3 = Directory::remove(path) || !Directory::exists(path);
    return checkResult(t1 && t2 && t3, "TestDirectoryCreateRemoveExists");
  }
  bool TestDirectoryGetAndSetCurrentDirectory()
  {
    bool t1, t2, t3;
    std::string currPath = Directory::getCurrentDirectory();
    std::string expects = Path::getFullFileSpec(".");
    t1 = expects == currPath;
    Directory::setCurrentDirectory(testPath_);
    std::string newPath = Directory::getCurrentDirectory();
    std::string tstPath = Path::getFullFileSpec(testPath_);
    t2 = newPath == tstPath;
    Directory::setCurrentDirectory(currPath);
    t3 = Directory::getCurrentDirectory() == currPath;
    return checkResult(t1 && t2 && t3, "TestDirectoryGetAndSetCurrentDirectory");
  }
  bool TestDirectoryGetFilesAndDirectories()
  {
    bool t1, t2, t3, t4, t5;

    // test finding files by copying know files then getting them
    std::string srcPath = ".";
    std::string dstPath = testPath_ + "/testDir1";

    // create place to look
    Directory::create(dstPath);
    // store files there
    std::string fileSpec1 = Path::fileSpec(srcPath,"FileSystem.h");
    std::string fileSpec2 = Path::fileSpec(srcPath,"FileSystem.cpp");
    std::string dstFile1 = Path::fileSpec(dstPath,"FileSystem.h"); 
    File::copy(fileSpec1, dstFile1);
    std::string dstFile2 = Path::fileSpec(dstPath,"FileSystem.cpp");
    File::copy(fileSpec2, dstFile2);
    t1 = File::exists(dstFile1) && File::exists(dstFile2);
    // now test getFiles
    std::vector<std::string> files = Directory::getFiles(dstPath);
    std::vector<std::string>::iterator iter 
      = std::find(files.begin(), files.end(), "FileSystem.h");
    t2 = iter != files.end();
    iter = std::find(files.begin(), files.end(), "FileSystem.cpp");
    t3 = iter != files.end();

    // test finding directories by creating them and then finding them
    Directory::create(testPath_ + "/testDir2");
    Directory::create(testPath_ + "/testDir3");
    std::vector<std::string> dirs = Directory::getDirectories(testPath_);
    iter = std::find(dirs.begin(), dirs.end(), "testDir2");
    t4 = iter != dirs.end();
    iter = std::find(dirs.begin(), dirs.end(), "testDir3");
    t5 = iter != dirs.end();

    // clean up
    Directory::remove(testPath_ + "/testDir1");  // fails because it has contents
    Directory::remove(testPath_ + "/testDir2");  
    Directory::remove(testPath_ + "/testDir3");

    return checkResult(t1 && t2 && t3 && t4 &&t5, "TestDirectoryGetFilesAndDirectories");
  }
  bool test()  // overriding inherited test method
  {
    bool t1, t2, t3;
    t1 = doTest(&TestDirectory::TestDirectoryCreateRemoveExists);
    t2 = doTest(&TestDirectory::TestDirectoryGetAndSetCurrentDirectory);
    t3 = doTest(&TestDirectory::TestDirectoryGetFilesAndDirectories);
    return t1 && t2 && t3;
  }
private:
  std::string testPath_;
};

bool test()
{
  bool t1, t2, t3, t4;
  TestFile tf;
  t1 = tf.test();
  TestFileInfo tfi;
  t2 = tfi.test();
  TestPath tp;
  t3 = tp.test();
  TestDirectory td;
  t4 = td.test();
  return t1 && t2 && t3;
}
int main(int argc, char* argv[])
{
  std::cout << "\n  Testing FileSystem namespace classes";
  std::cout << "\n ======================================";

  if(test())
    std::cout << "\n\n  all tests passed\n\n";
  else
    std::cout << "\n\n  at least one test failed\n\n";
}