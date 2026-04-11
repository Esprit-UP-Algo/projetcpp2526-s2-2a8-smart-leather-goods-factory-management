#include "floatingaibutton.h"

FloatingAIButton::FloatingAIButton(QWidget *parent)
    : QPushButton(parent)
{
    setText("🤖");
    setFixedSize(56, 56);
    setStyleSheet(
        "QPushButton {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 28px;"
        "  font-size: 22px;"
        "}"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton:pressed { background-color: #6D4C41; }"
    );
    setToolTip("Assistant IA");
    raise();
}
