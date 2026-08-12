#include <QApplication>

#include "ui/MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    icodental::ui::MainWindow window;
    window.show();

    return application.exec();
}