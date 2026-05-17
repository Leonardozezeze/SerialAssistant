#ifndef LABELEDCOMBOBOX_H
#define LABELEDCOMBOBOX_H
#include <QtWidgets>
class LabeledComboBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)
    Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText)
public:
    explicit LabeledComboBox(const QString &label = "", QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);  // 无边距
        m_label=new QLabel(label,this);
        m_comboBox = new QComboBox(this);

        // 设置默认大小
        m_comboBox->setMinimumWidth(40);   // 默认最小40像素
        m_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        
        // 标签默认右对齐，更美观
        m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_label->setMinimumWidth(30);  // 标签最小宽度

        layout->addWidget(m_label);
        layout->addWidget(m_comboBox);
    }
    void addItem(const QString &text) { m_comboBox->addItem(text); }
    void addItems(const QStringList &items) { m_comboBox->addItems(items); }

    QString currentText() const { return m_comboBox->currentText(); }
    void setCurrentText(const QString &text) { m_comboBox->setCurrentText(text); }

    int currentIndex() const { return m_comboBox->currentIndex(); }
    void setCurrentIndex(int index) { m_comboBox->setCurrentIndex(index); }

    QString labelText() const { return m_label->text(); }
    void setLabelText(const QString &text) { m_label->setText(text); }

    QComboBox* comboBox() { return m_comboBox; }  // 供高级操作

private:
    QLabel *m_label;
    QComboBox *m_comboBox;
};
#endif