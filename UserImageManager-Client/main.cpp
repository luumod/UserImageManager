#include "SApp.h"
#include "LoginRegisterDlg.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    SApp app(argc, argv);

    {
        LoginRegister dlg;
        if (dlg.exec() == QDialog::Rejected) {
            return 0;
        }
    }
    
    MainWindow window;
    window.show();

    return app.exec();
}