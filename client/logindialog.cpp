/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "logindialog.h"

#include <QtCore/QDebug>
#include <QtWidgets/QFormLayout>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    m_connection = 0;
    
    serverEdit = new QLineEdit();
    userEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode( QLineEdit::Password );
    sessionLabel = new QLabel("Session:");
    loginButton = new QPushButton("Login");
    
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Server", serverEdit);
    formLayout->addRow("User", userEdit);
    formLayout->addRow("Password", passwordEdit);
    
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(sessionLabel);
    
    setLayout(mainLayout);
    
    connect( loginButton, &QPushButton::clicked, this, &LoginDialog::login );
}

QMatrixClient::Connection* LoginDialog::connection() const
{
    return m_connection;
}

QString LoginDialog::token() const
{
    return m_token;
}

QString LoginDialog::homeServer() const
{
    return m_homeServer;
}

QString LoginDialog::userId() const
{
    return m_userId;
}

void LoginDialog::login()
{
    qDebug() << "login";
    QUrl url = QUrl::fromUserInput(serverEdit->text());
    QString user = userEdit->text();
    QString password = passwordEdit->text();
    m_connection = new QMatrixClient::Connection(url);
    QMatrixClient::PasswordLogin* job = new QMatrixClient::PasswordLogin(m_connection, user, password);
    connect( job, &QMatrixClient::PasswordLogin::result, this, &LoginDialog::loginDone );
    job->start();
}

void LoginDialog::loginDone(KJob* job)
{
    QMatrixClient::PasswordLogin* realJob = static_cast<QMatrixClient::PasswordLogin*>(job);
    if( realJob->error() )
    {
        sessionLabel->setText( realJob->errorText() );
    }
    else
    {
        sessionLabel->setText( "Token: " + realJob->token() );
        qDebug() << realJob->token();
        m_token = realJob->token();
        m_userId = realJob->id();
        m_homeServer = realJob->server();
        accept();
    }
}