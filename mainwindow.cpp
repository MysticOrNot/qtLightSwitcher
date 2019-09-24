#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GlobalActionHelper.h"
#include <QClipboard>
#include <QMimeData>
#include <QSystemTrayIcon>
#include <QWidget>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowIcon(QIcon("./switch.png"));
    ui->setupUi(this);

    // горячие клавиши
    GlobalActionHelper::init ();
    globalAction = new QAction(this);
    globalAction->setShortcut(QKeySequence ("Alt+T"));
    connect(globalAction, &QAction::triggered, [=](){
        hotKeyPressed();
    });
    GlobalActionHelper::makeGlobal (globalAction);
    initPasteKeys();

    //connect( ui->bnShowMsg, SIGNAL( clicked( bool ) ), SLOT( onShowMessageInTray() ) );

    trayIcon = new QSystemTrayIcon( QIcon("./switch.png"), this );
    connect(
         trayIcon,
         SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
         SLOT( onTrayIconActivated( QSystemTrayIcon::ActivationReason ) )
    );

    trayIconMenu = new QMenu;

    quitAction = trayIconMenu->addAction( "Выход" );
    connect( quitAction, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );

    minimizeAction = trayIconMenu->addAction( "Свернуть" );
    connect( minimizeAction, SIGNAL( triggered( bool ) ), this, SLOT( hide() ) );

    restoreAction = trayIconMenu->addAction( "Показать" );
    connect( restoreAction, SIGNAL( triggered( bool ) ), this, SLOT( showNormal() ) );

    trayIcon->setContextMenu( trayIconMenu );
    trayIcon->show();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void MainWindow::on_pushButton_clicked()
{
    changeLang(&eng, &rus);
}

QString MainWindow::changeLang(QString txt, QString *FromLang, QString *ToLang)
{
    QString res = "";

    for(int i = 0; i< txt.length(); i++){
        int ind = FromLang->indexOf(txt[i]);

        if (ind >= 0) {
            res += ToLang->at(ind);
        }
        else {
            res += txt[i];
        }
    }
    return res;
}

void MainWindow::changeLang(QString *FromLang, QString *ToLang)
{
    QString s = ui->plainTextEdit->toPlainText();

    ui->plainTextEdit->setPlainText(
        changeLang(s,
                   FromLang,
                   ToLang
        )
    );
}


void MainWindow::hotKeyPressed()
{

   // Copy();
    QString selectedText = getSelectedText();
    if (selectedText == NULL){
        return;
    }

    QString *FromLang = nullptr;
    QString *ToLang = nullptr;

    if (isLatin(selectedText)){
        FromLang = &eng;
        ToLang   = &rus;
    }
    else{
        FromLang = &rus;
        ToLang   = &eng;
    }

    QString chandelocaleStr = changeLang(selectedText, FromLang, ToLang);

    ui->plainTextEdit->setPlainText(chandelocaleStr);

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->clear();
    clipboard->setText(chandelocaleStr, QClipboard::Clipboard);
    clipboard->dataChanged();
    Paste();
    trayIcon->showMessage( "lightSwitcher", chandelocaleStr, QSystemTrayIcon::Information );
}

void MainWindow::initPasteKeys()
{
    Display *display;
    display=XOpenDisplay(NULL);

    controlKey = XKeysymToKeycode(display, XK_Control_L);
    shiftKey   = XKeysymToKeycode(display, XK_Shift_L);
    insertKey  = XKeysymToKeycode(display, XK_Insert);
    altKey     = XKeysymToKeycode(display, XK_Alt_L);
    vKey       = XKeysymToKeycode(display, XK_V);
    cKey       = XKeysymToKeycode(display, XK_C);

    XCloseDisplay(display);
}

QString MainWindow::getSelectedText()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Selection);

        if (mimeData->hasImage()) {
            //setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
            return NULL;
        } else if (mimeData->hasHtml()) {
            //setText(mimeData->html());
            //setTextFormat(Qt::RichText);
            QString s = clipboard->text(QClipboard::Selection);
            clipboard->destroyed();
            return s;
        } else if (mimeData->hasText()) {
            //setText(mimeData->text());
            //setTextFormat(Qt::PlainText);
            QString s = clipboard->text(QClipboard::Selection);
            clipboard->destroyed();
            return s;
        } else {
            return NULL;
        }
}

bool MainWindow::isLatin(QString txt)
{
    int engChars = 0;
    int rusChars = 0;
    for (int i = 0; i < txt.length(); i++){
        if (rus.indexOf(txt[i]) >= 0) rusChars++;
        else engChars++;
    }
    return engChars > rusChars;
}

void MainWindow::Paste()
{
    keyboart_event(controlKey, vKey);
}

void MainWindow::Copy()
{
    keyboart_event(controlKey, cKey);
}

void MainWindow::keyboart_event(unsigned int firstKey, unsigned int lastKey)
{
    Display *display;
    display=XOpenDisplay(nullptr);

    unsigned long currKey = XKeysymToKeycode(display, XK_T);



    XTestFakeKeyEvent(display, altKey,  False, CurrentTime);
    XTestFakeKeyEvent(display, currKey, False, CurrentTime);
    XFlush(display);

    XTestFakeKeyEvent(display, firstKey,  True, CurrentTime);
    XTestFakeKeyEvent(display, lastKey,   True, CurrentTime);
    XTestFakeKeyEvent(display, lastKey,   False, CurrentTime);
    XTestFakeKeyEvent(display, firstKey,  False, CurrentTime);

    XFlush(display);

    XCloseDisplay(display);
}


void MainWindow::on_pushButton_2_clicked()
{
   changeLang(&rus, &eng);
}

void MainWindow::on_plainTextEdit_textChanged()
{

}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch( reason ) {
        case QSystemTrayIcon::Trigger:
            setVisible( !isVisible() );
            break;

        default:
            break;
    }
}

void MainWindow::onShowMessageInTray()
{
    trayIcon->showMessage( "Message title", "Message text", QSystemTrayIcon::Information );
}
