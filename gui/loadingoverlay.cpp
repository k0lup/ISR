#include "loadingoverlay.h"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QFont>

LoadingOverlay::LoadingOverlay(QWidget* parent)
    : QWidget(parent)
{
    // ВАЖНО: overlay должен быть ребёнком окна, чтобы быть "поверх"
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_StyledBackground, true);

    // Чтобы ловить мышь и блокировать взаимодействие с underlying UI
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Полупрозрачный фон
    setStyleSheet("LoadingOverlay { background-color: rgba(0,0,0,120); }");

    // Центральная панель
    m_panel = new QWidget(this);
    m_panel->setObjectName("panel");
    m_panel->setStyleSheet(
        "#panel { background: white; border-radius: 10px; }"
        "QLabel { color: #222; }"
    );

    auto* panelLayout = new QVBoxLayout(m_panel);
    panelLayout->setContentsMargins(18, 16, 18, 16);
    panelLayout->setSpacing(10);

    m_title = new QLabel("Загрузка…", m_panel);
    QFont f = m_title->font();
    f.setPointSize(f.pointSize() + 2);
    f.setBold(true);
    m_title->setFont(f);

    m_message = new QLabel(m_panel);
    m_message->setWordWrap(true);
    m_message->setText("Подготовка данных…");

    m_bar = new QProgressBar(m_panel);
    m_bar->setRange(0, 100);
    m_bar->setValue(0);
    m_bar->setTextVisible(true);

    panelLayout->addWidget(m_title);
    panelLayout->addWidget(m_message);
    panelLayout->addWidget(m_bar);

    // Сам overlay занимает весь parent
    if (parent) {
        parent->installEventFilter(this);
        setGeometry(parent->rect());
    }

    hide();
}

void LoadingOverlay::setMessage(const QString& msg)
{
    m_message->setText(msg);
}

void LoadingOverlay::setProgress(int value)
{
    if (m_bar->minimum() == 0 && m_bar->maximum() == 0) {
        // если был busy-mode, вернём нормальный
        m_bar->setRange(0, 100);
    }
    m_bar->setValue(value);
}

void LoadingOverlay::setIndeterminate(bool on)
{
    if (on) {
        m_bar->setRange(0, 0); // Qt "busy"
    } else {
        m_bar->setRange(0, 100);
    }
}

bool LoadingOverlay::event(QEvent* e)
{
    // Блокируем любые клики/колесо/клавиатуру, чтобы под ним ничего не нажималось
    switch (e->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::Wheel:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::Shortcut:
        case QEvent::ShortcutOverride:
            return true; // съели
        default:
            break;
    }
    return QWidget::event(e);
}

void LoadingOverlay::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    centerPanel();
}

void LoadingOverlay::centerPanel()
{
    if (!m_panel) return;

    const int w = qMin(520, width() - 40);
    m_panel->setFixedWidth(w);

    // Высота по размеру контента
    m_panel->adjustSize();
    const QSize s = m_panel->sizeHint();
    m_panel->resize(w, s.height());

    m_panel->move((width() - m_panel->width()) / 2,
                  (height() - m_panel->height()) / 2);
}

bool LoadingOverlay::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == parentWidget()) {
        if (e->type() == QEvent::Resize || e->type() == QEvent::Move) {
            setGeometry(parentWidget()->rect());
            centerPanel();
        }
    }
    return QWidget::eventFilter(obj, e);
}
