/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtNetwork>

#include "client.h"

//! [0]
Client::Client(QWidget *parent)
    : QDialog(parent)
    , hostCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , getFortuneButton(new QPushButton(tr("Get Fortune")))
    , tcpSocket(new QTcpSocket(this))
    , consoleLog(new QTextEdit)
    , clearConsoleButton(new QPushButton(tr("Clear")))
    , fileDirLineEdit(new QLineEdit)
    , browseButton (new QPushButton(tr("Browse...")))
    , loginButton (new QPushButton(tr("Login")))
    , pwdButton (new QPushButton(tr("Test String")))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//! [0]
    hostCombo->setEditable(true);
    // find out name of this machine
//    QString name = QHostInfo::localHostName();
//    if (!name.isEmpty()) {
//        hostCombo->addItem(name);
//        QString domain = QHostInfo::localDomainName();
//        if (!domain.isEmpty())
//            hostCombo->addItem(name + QChar('.') + domain);
//    }
//    if (name != QLatin1String("localhost"))
//        hostCombo->addItem(QString("localhost"));
//    // find out IP addresses of this machine
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
//    // add non-localhost addresses
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (!ipAddressesList.at(i).isLoopback())
//            hostCombo->addItem(ipAddressesList.at(i).toString());
//    }
//    // add localhost addresses
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (ipAddressesList.at(i).isLoopback())
//            hostCombo->addItem(ipAddressesList.at(i).toString());
//    }

//    hostCombo->addItem(QString("www.stroustrup.com")); //for temporary telnet testing, port 80.
    hostCombo->addItem(QString("192.168.100.118")); //for temporary telnet testing, port 80.
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    portLineEdit->setText(QString("23"));
    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(portLineEdit);
    auto fileDirLabel = new QLabel(tr("Directory:"));
    fileDirLabel->setBuddy(fileDirLineEdit);

    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));


    fileDirLineEdit->setText(QString(QDir::currentPath()));

    getFortuneButton->setDefault(true);
    getFortuneButton->setEnabled(true);

    auto quitButton = new QPushButton(tr("Quit"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

//! [1]
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
//! [1]

    connect(hostCombo, &QComboBox::editTextChanged,
            this, &Client::enableGetFortuneButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &Client::enableGetFortuneButton);
    connect(getFortuneButton, &QAbstractButton::clicked,
            this, &Client::requestNewFortune);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
//! [2] //! [3]
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::readFortune);
//! [2] //! [4]
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
//! [3]
            this, &Client::displayError);
//! [4]
    connect(clearConsoleButton, &QAbstractButton::clicked,
            this, &Client::clearConsole);
    connect(browseButton, &QAbstractButton::clicked,
            this, &Client::openFileDialog);
    connect(loginButton, &QAbstractButton::clicked,
            this, &Client::enterLogin);
    connect(pwdButton, &QAbstractButton::clicked,
            this, &Client::enterPwd);
    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QGridLayout(this);
    }
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);

    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);

    mainLayout->addWidget(fileDirLabel, 2, 0);
    mainLayout->addWidget(fileDirLineEdit, 2, 1);
    mainLayout->addWidget(browseButton,2,2);

    mainLayout->addWidget(statusLabel, 3, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 4, 0, 1, 2);
    mainLayout->addWidget(consoleLog,5,0);
    mainLayout->addWidget(clearConsoleButton,5,1);
    mainLayout->addWidget(loginButton,6,0);
    mainLayout->addWidget(pwdButton,6,1);
    setWindowTitle(QGuiApplication::applicationDisplayName());
//    portLineEdit->setFocus();

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Client::sessionOpened);

        getFortuneButton->setEnabled(true);
        statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    }
//! [5]
}
//! [5]

//! [6]
void Client::requestNewFortune()
{
    getFortuneButton->setEnabled(false);
    tcpSocket->abort();
//! [7]
    tcpSocket->connectToHost(hostCombo->currentText(),
                             portLineEdit->text().toInt());
//! [7]
// QString requestString ="GET /index.html HTTP/1.1\r\nhost: "+
//    hostCombo->currentText()+"\r\n\r\n";
QString requestString = "";
    QByteArray ba;
    ba.append(requestString);
    tcpSocket->write(ba);
}
//! [6]

//! [8]
void Client::readFortune()
{
//    in.startTransaction();

//    QString nextFortune;
//    in >> nextFortune;

//    if (!in.commitTransaction())
//        return;
//    consoleLog->append(nextFortune);
//    if (nextFortune == currentFortune) {
//        QTimer::singleShot(0, this, &Client::requestNewFortune);
//        return;
//    }

//    currentFortune = nextFortune;
//    statusLabel->setText(currentFortune);

    getFortuneButton->setEnabled(true);

    quint16 bytes =  tcpSocket->bytesAvailable();
//    consoleLog->append("----------------------------------------");
//    consoleLog->append(QString::number(bytes)+ " for you to read");
//    consoleLog->append("----------------------------------------");

    /*
    * When you read data from a socket ,data is also removed from the socket buffer space.
    *
    */
    QByteArray ba= tcpSocket->readAll();
    QString dataString (ba);
    consoleLog->append(dataString);
    if(dataString.contains("HTTP")){
        consoleLog->append("HTTP found!");
    }

    //write to csv
    double value1(10);
    double value2(13.2);
    QFile file("./file.csv");
    if (file.open(QFile::WriteOnly|QFile::Append))
    {
    QTextStream stream(&file);
    stream << value1 << "," << value2 << "\n"; // this writes first line with two columns
    file.close();
    }

}
//! [8]

void Client::clearConsole()
{
    consoleLog->clear();
}

void Client::openFileDialog()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
}


void Client::enterLogin(){
    QString requestString = "episode\r\n";
        QByteArray ba;
        ba.append(requestString);
        tcpSocket->write(ba);
}

void Client::enterPwd(){
    QString requestString = "FE C0 04 C2\r\n";
        QByteArray ba;
        ba.append(requestString);
        tcpSocket->write(ba);
}

//! [13]
void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    getFortuneButton->setEnabled(true);
}
//! [13]

void Client::enableGetFortuneButton()
{
    getFortuneButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                                 !hostCombo->currentText().isEmpty() &&
                                 !portLineEdit->text().isEmpty());

}



void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    statusLabel->setText(tr("This examples requires that you run the "
                            "Fortune Server example as well."));

    enableGetFortuneButton();
}

