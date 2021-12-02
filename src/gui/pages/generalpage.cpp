//
// Created by nils on 11/5/21.
//

#include "generalpage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QRadioButton>

#include "gui/widgets/filechooser.h"

GeneralPage::GeneralPage(Config& config, QWidget *parent) : ConfigurationPage(config, parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(40);

    keepMemorySame = new QCheckBox(QStringLiteral("Keep initial and maximum memory allocations the same"));

    QLabel* initialMemoryLabel = new QLabel();
    initialMemory = new QSlider(Qt::Horizontal);
    initialMemory->setMinimum(1024);
    initialMemory->setMaximum(16384);
    initialMemory->setPageStep(1024);

    QLabel* maxMemoryLabel = new QLabel();
    maxMemory = new QSlider(Qt::Horizontal);
    maxMemory->setMinimum(1024);
    maxMemory->setMaximum(16384);
    maxMemory->setPageStep(1024);

    //Memory slider functionality
    connect(initialMemory, &QSlider::valueChanged, [initialMemoryLabel](int val){initialMemoryLabel->setText("Initial Memory:  " + QString::number(val) + " MiB");});
    connect(maxMemory, &QSlider::valueChanged, [maxMemoryLabel](int val){maxMemoryLabel->setText("Maximum Memory:  " + QString::number(val) + " MiB");});

    connect(keepMemorySame, &QCheckBox::toggled, this, &GeneralPage::keepMinMaxSameChanged);

    //Memory slider group
    QVBoxLayout* memorySliderContainer = new QVBoxLayout();
    memorySliderContainer->setSpacing(6);

    memorySliderContainer->addWidget(initialMemoryLabel, 0, Qt::AlignHCenter);
    memorySliderContainer->addWidget(initialMemory);
    memorySliderContainer->addSpacing(30);
    memorySliderContainer->addWidget(maxMemoryLabel, 0, Qt::AlignHCenter);
    memorySliderContainer->addWidget(maxMemory);

    //Custom jre checkbox lineedit and button
    useCustomJre = new QCheckBox(QStringLiteral("Use custom jre"));

    jrePath = new FileChooser(FileChooser::FileType::FILE);
    jrePath->setDisabled(true);

    connect(useCustomJre, &QCheckBox::toggled, jrePath, &QLineEdit::setEnabled);

    //Custom jre groups
    QVBoxLayout* customJreContainer = new QVBoxLayout();
    customJreContainer->setSpacing(6);
    customJreContainer->addWidget(useCustomJre, 0, Qt::AlignHCenter);
    customJreContainer->addWidget(jrePath);

    //Jvm arguments
    QVBoxLayout* jvmArgsGroup = new QVBoxLayout();
    jvmArgsGroup->setSpacing(6);

    jvmArgs = new QPlainTextEdit();

    jvmArgsGroup->addWidget(new QLabel(QStringLiteral("JVM Arguments")), 0, Qt::AlignHCenter);
    jvmArgsGroup->addWidget(jvmArgs);

    //Checkboxes
    QGroupBox* groupBox = new QGroupBox(QStringLiteral("After Launch"));

    QRadioButton* stayOpen = new QRadioButton(QStringLiteral("Keep launcher open"));
    QRadioButton* close = new QRadioButton(QStringLiteral("Close launcher"));
    stayOpen->setChecked(true);

    QVBoxLayout* radioLayout = new QVBoxLayout();
    radioLayout->setSpacing(6);
    radioLayout->addWidget(stayOpen);
    radioLayout->addWidget(close);
    groupBox->setLayout(radioLayout);


    mainLayout->addWidget(keepMemorySame, 0, Qt::AlignHCenter);
    mainLayout->addLayout(memorySliderContainer);
    mainLayout->addLayout(customJreContainer);
    mainLayout->addLayout(jvmArgsGroup, 1);
    mainLayout->addWidget(groupBox);


    setLayout(mainLayout);
}

QString GeneralPage::title() {
    return QStringLiteral("General");
}

QIcon GeneralPage::icon() {
    return QIcon(":/res/icons/cog.svg");
}

void GeneralPage::apply() {
    config.keepMemorySame = keepMemorySame->isChecked();
    config.initialMemory = initialMemory->value();
    config.maximumMemory = maxMemory->value();

    config.useCustomJre = useCustomJre->isChecked();
    config.customJrePath = jrePath->getPath();

    config.jvmArgs = jvmArgs->toPlainText();
}

void GeneralPage::load() {
    keepMemorySame->setChecked(config.keepMemorySame);

    initialMemory->setValue(config.initialMemory);
    maxMemory->setValue(config.maximumMemory);

    useCustomJre->setChecked(config.useCustomJre);
    jrePath->setPath(config.customJrePath);

    jvmArgs->setPlainText(config.jvmArgs);
}


void GeneralPage::keepMinMaxSameChanged(bool checked) {
    if(checked){
        initialMemory->setValue(maxMemory->value());
        maxMemory->setDisabled(true);
        connect(initialMemory, &QSlider::valueChanged, maxMemory, &QSlider::setValue, Qt::UniqueConnection);
    }else{
        maxMemory->setEnabled(true);
        disconnect(initialMemory, &QSlider::valueChanged, maxMemory, &QSlider::setValue);
    }
}