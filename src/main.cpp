#include <QApplication>
#include <QMessageBox>

#include "bootstrap/ApplicationServices.hpp"
#include "ui/viewmodels/MainViewModel.hpp"
#include "ui/MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    icodental::bootstrap::ApplicationServices services(icodental::bootstrap::ApplicationServices::defaultCacheDatabasePath());

    if (!services.initialize()) {
        QMessageBox::critical(
            nullptr,
            "IcoDental startup error",
            QString("Unable to initialize the local cache database.\n\n%1")
                .arg(services.lastError()));
        return 1;
    }

    icodental::ui::MainViewModel viewModel(services.analysisOrchestrator());
    icodental::ui::MainWindow window(viewModel);
    window.show();

    return application.exec();
}