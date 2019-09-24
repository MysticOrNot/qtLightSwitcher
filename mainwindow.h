#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_plainTextEdit_textChanged();

private:
    Ui::MainWindow *ui;
    QString rus  = "йцукенгшщзхъфывапролджэячсмитьбюЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ!\"№;%:?*()_+";
    QString eng  = "qwertyuiop[]asdfghjkl;'zxcvbnm,.QWERTYUIOP[]ASDFGHJKL;'ZXCVBNM,.!@#$%^&*()_+";
    void changeLang(QString *FromLang, QString *ToLang);
    QString changeLang(QString txt, QString *FromLang, QString *ToLang);
    QAction *globalAction;
    void hotKeyPressed();
    void Paste();
    void Copy();
    void keyboart_event(unsigned int firstKey, unsigned int lastKey);
    void initPasteKeys();
    QString getSelectedText();
    bool isLatin(QString txt);
    unsigned int controlKey;
    unsigned int shiftKey;
    unsigned int insertKey;
    unsigned int altKey;
    unsigned int vKey;
    unsigned int cKey;

};

#endif // MAINWINDOW_H
