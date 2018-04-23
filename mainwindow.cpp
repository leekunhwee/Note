#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QMessageBox>
#include  <QFileDialog>
#include  <QTextStream>

#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include <QCloseEvent>

#include <QDialog>

void MainWindow::newFile()
{
    if (maybeSave()){
        isUntitled = true;
        curFile = tr("未命名.txt");
        setWindowTitle(curFile);
        ui->textEdit->clear();
        ui->textEdit->setVisible(true);
    }
}

bool MainWindow::maybeSave()
{
    //如果文档被更改了
    if(ui->textEdit->document()->isModified()){
        //自定义一个警告对话框
        QMessageBox box;
        box.setWindowTitle(tr("警告"));
        box.setIcon(QMessageBox::Warning);
        box.setText(curFile + tr("尚未保存，是否保存？"));
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("取消"),QMessageBox::RejectRole);

        box.exec();
        if (box.clickedButton() == yesBtn)
            return save();
        else if (box.clickedButton() == cancelBtn)
            return false;
    }
    //如果文档没有被修改，直接返回true
    return true;
}

bool MainWindow::save()
{
    if (isUntitled){
        return saveAs();
    }else{
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),curFile,tr("文本文件(*txt)"));
    if(fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly| QFile::Text)){

        //%1和%2分别对应后面arg两个参数，\n起换行的作用
        QMessageBox::warning(this,tr("多文档编辑器"),
                             tr("无法写入文件 %1: \n %2 ")
                             .arg(fileName) .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    //鼠标指针变成等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<ui->textEdit->toPlainText();
    //鼠标恢复原来状态
    QApplication::restoreOverrideCursor();
    isUntitled = false;
    //获得文件的标准路径
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);//新建QFile对象

    if (!file.open(QFile::ReadOnly| QFile::Text)){

        //%1和%2分别对应后面arg两个参数，\n起换行的作用
        QMessageBox::warning(this,tr("多文档编辑器"),
                             tr("无法读取文件 %1: \n %2 ")
                             .arg(fileName) .arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);//新建文本流对象
    //鼠标指针变成等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //读取文件的全部文本内容，并添加到编辑器
    ui->textEdit->setPlainText(in.readAll());
    //鼠标恢复原来状态
    QApplication::restoreOverrideCursor();
    isUntitled = false;

    //设置当前文件
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //  初始化文件为未保存状态
    isUntitled = true;
    //  初始化文件名为"未命名.txt"
    curFile = tr("未命名.txt");
    //  初始化窗口标题为文件名
    setWindowTitle(curFile);

    QPushButton *btn = new QPushButton(this);
    QLineEdit *le = new QLineEdit(this);

    //新建栅格布局管理器
    QGridLayout *layout = new QGridLayout;
    //添加部件，从第0行0列开始，占据1行1列
    layout->addWidget(btn,0,0,1,1);
    //添加部件，从第0行1列开始，占据1行2列
    layout->addWidget(le,0,1,1,2);
    //添加部件，从第1行0列开始，占据1行3列
    layout->addWidget(ui->textEdit,1,0,1,3);
    ui->centralWidget->setLayout(layout);


    findDlg = new QDialog(this);
        findDlg->setWindowTitle(tr("查找"));
        findLineEdit = new QLineEdit(findDlg);
        QPushButton *btn1= new QPushButton(tr("查找下一个"),findDlg);
        QVBoxLayout *layout1= new QVBoxLayout(findDlg);
        layout1->addWidget(findLineEdit);
        layout1->addWidget(btn1);
        connect(btn1, &QPushButton::clicked, this, &MainWindow::showFindText);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_triggered()
{
    newFile();
}

void MainWindow::on_action_Save_triggered()
{
    save();
}

void MainWindow::on_action_SaveAs_triggered()
{
    saveAs();
}

void MainWindow::on_action_Open_triggered()
{
    if (maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this);
        if(!fileName.isEmpty()){
            loadFile(fileName);
            ui->textEdit->setVisible(true);
        }
    }
}

void MainWindow::on_action_Close_triggered()
{
     ui->textEdit->setVisible(false);
}

void MainWindow::on_action_Exit_triggered()
{
    //先执行关闭操作，再退出程序
    //qApp是指向应用程序的全局指针
    on_action_Close_triggered();
    qApp->quit();
}


void MainWindow::on_action_Undo_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::on_action_Cut_triggered()
{
    ui->textEdit->cut();
}

void MainWindow::on_action_Copy_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_action_Paste_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::showFindText()
{
    QString str  =  findLineEdit->text();
    if  (!ui->textEdit->find(str, QTextDocument::FindBackward))
    {
        QMessageBox::warning(this, tr("查找"), tr("找不到%1").arg(str)); }
}

void MainWindow::on_action_Find_triggered()
{
    findDlg->show();
}
