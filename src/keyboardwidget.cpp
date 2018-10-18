#include "keyboardwidget.h"
#include "ui_keyboardwidget.h"
#include <QDebug>
#include <QFile>

#define SYMBOL_KEY_COUNT 29
#define SYMBOL_PAGE_COUNT 2

QChar symbols[SYMBOL_PAGE_COUNT][SYMBOL_KEY_COUNT] =
              { {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
                'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
                'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'},
                {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
                 '`', '-', '=', '[', ']', '\\', '|', '{', '}',
                 '~','<', '>', ':', ';', '\'', '"', '-', '+', '?'}};


KeyboardWidget::KeyboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyboardWidget),
    capsLock(false),
    isShift(false),
    page(0)
{
    vKeyboard = new X11Keyboard(this);
    connect(this, SIGNAL(keyPressed(QChar)), vKeyboard, SLOT(onKeyPressed(QChar)));
    connect(this, SIGNAL(keyPressed(FuncKey)), vKeyboard, SLOT(onKeyPressed(FuncKey)));

    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::WindowDoesNotAcceptFocus);

    QFile qssFile(":/qss/keyboard.qss");
    qssFile.open(QIODevice::ReadOnly);
    setStyleSheet(qssFile.readAll());
    qssFile.close();

    bindSingal();
}

KeyboardWidget::~KeyboardWidget()
{
    delete ui;
}

void KeyboardWidget::resizeEvent(QResizeEvent */*event*/)
{
    int w = width();
    int mainLeftMargin = ui->hl_main->contentsMargins().left();
    int mainRightMargin = ui->hl_main->contentsMargins().right();
    int mainSpacing = ui->hl_main->spacing();
    int itemSpacing = ui->hl_1->spacing();

    int btnWidthCount = w - 11 * itemSpacing - mainSpacing- mainLeftMargin - mainRightMargin;
    double btnWidth = btnWidthCount / 12;
    for(int i = 0; i <= 28; i++) {
        QString btnObjName = "btn_" + QString::number(i);
        QPushButton *btn = ui->page_letter->findChild<QPushButton*>(btnObjName);
        btn->setFixedWidth(btnWidth);
    }
    ui->btn_backspace->setFixedWidth(btnWidth);
    ui->btn_ctrl_l->setFixedWidth(btnWidth * 1.3);
    ui->btn_ctrl_r->setFixedWidth(btnWidth * 1.3);
    ui->btn_alt_l->setFixedWidth(btnWidth);
    ui->btn_alt_r->setFixedWidth(btnWidth);
    ui->btn_super->setFixedWidth(btnWidth);


    for(int i = 1; i <= 9; i++) {
        QString btnObjName = "btn_num_" + QString::number(i);
        QPushButton *btn = ui->page_number->findChild<QPushButton*>(btnObjName);
        btn->setFixedWidth(btnWidth);
    }
    ui->btn_insert->setFixedWidth(btnWidth);
    ui->btn_delete->setFixedWidth(btnWidth);
    ui->btn_home->setFixedWidth(btnWidth);
    ui->btn_end->setFixedWidth(btnWidth);
    ui->btn_pgup->setFixedWidth(btnWidth);
    ui->btn_pgdn->setFixedWidth(btnWidth);
    ui->btn_up->setFixedWidth(btnWidth);
    ui->btn_down->setFixedWidth(btnWidth);
    ui->btn_left->setFixedWidth(btnWidth);
    ui->btn_right->setFixedWidth(btnWidth);
}

void KeyboardWidget::bindSingal()
{
    for(auto obj : ui->page_letter->children()) {
        if(obj->metaObject()->className() == QString("QPushButton")) {
            QPushButton *btn = static_cast<QPushButton*>(obj);
            btn->setFocusPolicy(Qt::NoFocus);
            connect(btn, &QPushButton::clicked, this, &KeyboardWidget::onButtonClicked);
        }
    }
    for(auto obj : ui->page_number->children()) {
        if(obj->metaObject()->className() == QString("QPushButton")) {
            QPushButton *btn = static_cast<QPushButton*>(obj);
            btn->setFocusPolicy(Qt::NoFocus);
            connect(btn, &QPushButton::clicked, this, &KeyboardWidget::onButtonClicked);
        }
    }
    ui->btn_close->setFocusPolicy(Qt::NoFocus);
    ui->btn_letter->setFocusPolicy(Qt::NoFocus);
    ui->btn_symbol->setFocusPolicy(Qt::NoFocus);
    ui->btn_number->setFocusPolicy(Qt::NoFocus);

    connect(ui->btn_letter, &QPushButton::clicked, this, [&] {
        ui->stackedWidget->setCurrentWidget(ui->page_letter);
        page = 0;
        switchPage();
    });
    connect(ui->btn_symbol, &QPushButton::clicked, this, [&] {
        ui->stackedWidget->setCurrentWidget(ui->page_letter);
        page = 1;
        switchPage();
    });
    connect(ui->btn_number, &QPushButton::clicked, this, [&] {
        ui->stackedWidget->setCurrentWidget(ui->page_number);
    });
    connect(ui->btn_close, &QPushButton::clicked, this, &KeyboardWidget::close);
}

void KeyboardWidget::onButtonClicked()
{
    QObject *obj = sender();
    if(obj->metaObject()->className() != QString("QPushButton"))
        return;

    QPushButton *btn = static_cast<QPushButton*>(obj);
    QString objName = btn->objectName();
    QString keyName = objName.right(objName.length() - 4);
    qDebug() << "keyName: " << keyName;

    if(keyName == "shift_l" || keyName == "shift_r") {
        if(page == 0) {
            isShift = !isShift;
            if(isShift) {     //第一次被按下
                capsLock = false;
                shiftLastClicked = QTime::currentTime();
            }
            else {
                int doubleClickInterval = QApplication::doubleClickInterval();
                if(shiftLastClicked.msecsTo(QTime::currentTime()) <= doubleClickInterval) {
                    //shift键双击，锁定大写
                    capsLock = true;
                    isShift = true;
                }
            }
            qDebug() << isShift;
            if(isShift)
                vKeyboard->addModifier(SHIFT);
            else
                vKeyboard->removeModifier(SHIFT);
            toggleCase();
        } else {
            page = page % (SYMBOL_PAGE_COUNT - 1) + 1;
            switchPage();
        }
    } else if(keyName == "switch") {
        if(page == 0)
            page = 1;
        else
            page = 0;
        switchPage();
    } else if(keyName == "ctrl_l" || keyName == "ctrl_r")
        vKeyboard->addModifier(CTRL);
    else if(keyName == "alt_l" || keyName == "alt_r")
        vKeyboard->addModifier(ALT);
    else if(keyName == "super")
        vKeyboard->addModifier(SUPER);
    else if(keyName == "space")
        Q_EMIT keyPressed(SPACE);
    else if(keyName == "backspace")
        Q_EMIT keyPressed(BACKSPACE);
    else if(keyName == "enter")
        Q_EMIT keyPressed(ENTER);
    else if(keyName == "insert")
        Q_EMIT keyPressed(INSERT);
    else if(keyName == "delete")
        Q_EMIT keyPressed(DELETE);
    else if(keyName == "home")
        Q_EMIT keyPressed(HOME);
    else if(keyName == "end")
        Q_EMIT keyPressed(END);
    else if(keyName == "pgup")
        Q_EMIT keyPressed(PGUP);
    else if(keyName == "pgdn")
        Q_EMIT keyPressed(PGDN);
    else if(keyName == "up")
        Q_EMIT keyPressed(UP);
    else if(keyName == "down")
        Q_EMIT keyPressed(DOWN);
    else if(keyName == "left")
        Q_EMIT keyPressed(LEFT);
    else if(keyName == "right")
        Q_EMIT keyPressed(RIGHT);
    else {    //字符键
        QChar c;
        QString text = btn->text();
        qDebug() << "clicked button text: " << text;
        if(text == "&&")
            c = '&';
        else if(text.length() == 1)
            c = text.at(0);

        Q_EMIT keyPressed(c);

        //如果shift键被单击，按一个键后就恢复为小写
        if(isShift && !capsLock) {
            isShift = false;
            vKeyboard->removeModifier(SHIFT);
            toggleCase();
        }
    }
}

void KeyboardWidget::toggleCase()
{
    for(int i = 0; i < 26; i++) {
        QString objName = "btn_" + QString::number(i);
        QPushButton *btn = findChild<QPushButton*>(objName);
        QChar ch;
        if(isShift) {   //切换到大写
            ch = symbols[0][i].toUpper();
        } else {
            ch = symbols[0][i];
        }
        btn->setText(ch);
    }
}

void KeyboardWidget::switchPage()
{
    if(page == 0) {
        ui->btn_shift_l->setText("Shift");
        ui->btn_shift_r->setText("Shift");
    } else {
        QString text = QString("%1/%2").arg(page).arg(SYMBOL_PAGE_COUNT - 1);
        ui->btn_shift_l->setText(text);
        ui->btn_shift_r->setText(text);
    }

    for(int i = 0; i < SYMBOL_KEY_COUNT; i++) {
        QString btnObjName = "btn_" + QString::number(i);
        QPushButton *btn = ui->page_letter->findChild<QPushButton*>(btnObjName);
        QChar c = symbols[page][i];
        if(c == '&')
            btn->setText("&&");
        else
            btn->setText(c);
    }
}

