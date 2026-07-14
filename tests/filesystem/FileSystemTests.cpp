#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include "infrastructure/filesystem/FileScanner.hpp"
#include "infrastructure/filesystem/ImageHasher.hpp"
#include "infrastructure/filesystem/SupportedImageTypes.hpp"

namespace fs = icodental::infrastructure::filesystem;

class FileSystemTests : public QObject {
    Q_OBJECT

    private slots:
        void supportedImageTypes_acceptsSupportedExtensions();
        void supportedImageTypes_rejectsUnsupportedExtensions();
        void fileScanner_filtersOnlySupportedFiles();
        void fileScanner_scansDirectoryRecursively();
        void imageHasher_returnsStableHashForSameFile();

    private:
        static QString createFile(const QString& path, const QByteArray& content);
};

QString FileSystemTests::createFile(const QString& path, const QByteArray& content) {
    QFile file(path);
    const bool opened = file.open(QIODevice::WriteOnly);
    Q_ASSERT(opened);
    file.write(content);
    file.close();
    return path;
}

void FileSystemTests::supportedImageTypes_acceptsSupportedExtensions() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const auto jpgPath = createFile(tempDir.filePath("case1.jpg"), "a");
    const auto pngPath = createFile(tempDir.filePath("case2.PNG"), "b");
    const auto tifPath = createFile(tempDir.filePath("case3.tiff"), "c");

    QVERIFY(fs::SupportedImageTypes::isSupportedFile(jpgPath));
    QVERIFY(fs::SupportedImageTypes::isSupportedFile(pngPath));
    QVERIFY(fs::SupportedImageTypes::isSupportedFile(tifPath));
}

void FileSystemTests::supportedImageTypes_rejectsUnsupportedExtensions() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const auto txtPath = createFile(tempDir.filePath("note.txt"), "a");
    const auto pdfPath = createFile(tempDir.filePath("sheet.pdf"), "b");

    QVERIFY(!fs::SupportedImageTypes::isSupportedFile(txtPath));
    QVERIFY(!fs::SupportedImageTypes::isSupportedFile(pdfPath));
    QVERIFY(!fs::SupportedImageTypes::isSupportedFile(tempDir.filePath("missing.jpg")));
}

void FileSystemTests::fileScanner_filtersOnlySupportedFiles() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const auto jpgPath = createFile(tempDir.filePath("a.jpg"), "1");
    const auto txtPath = createFile(tempDir.filePath("b.txt"), "2");
    const auto pngPath = createFile(tempDir.filePath("c.png"), "3");

    fs::FileScanner scanner;
    const QStringList input{jpgPath, txtPath, pngPath, jpgPath};
    const auto result = scanner.filterSupportedFiles(input);

    QCOMPARE(result.size(), 2);
    QVERIFY(result.contains(QFileInfo(jpgPath).canonicalFilePath()));
    QVERIFY(result.contains(QFileInfo(pngPath).canonicalFilePath()));
}

void FileSystemTests::fileScanner_scansDirectoryRecursively() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QDir root(tempDir.path());
    QVERIFY(root.mkpath("nested"));

    const auto rootImage = createFile(tempDir.filePath("root.jpg"), "root");
    const auto nestedImage = createFile(tempDir.filePath("nested/inside.png"), "inside");
    createFile(tempDir.filePath("nested/skip.txt"), "skip");

    fs::FileScanner scanner;
    const auto result = scanner.scanDirectory(tempDir.path());

    QCOMPARE(result.size(), 2);
    QVERIFY(result.contains(QFileInfo(rootImage).canonicalFilePath()));
    QVERIFY(result.contains(QFileInfo(nestedImage).canonicalFilePath()));
}

void FileSystemTests::imageHasher_returnsStableHashForSameFile() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const auto imagePath = createFile(tempDir.filePath("stable.jpg"), "same-content");

    fs::ImageHasher hasher;
    const auto first = hasher.hashFile(imagePath);
    const auto second = hasher.hashFile(imagePath);

    QVERIFY(first.isValid());
    QVERIFY(second.isValid());
    QCOMPARE(first.sha256(), second.sha256());
}

QTEST_APPLESS_MAIN(FileSystemTests)

#include "FileSystemTests.moc"