#ifndef LOADINGOVERLAY_H
#define LOADINGOVERLAY_H

#include <QWidget>

class QLabel;
class QProgressBar;
class QVBoxLayout;

class LoadingOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingOverlay(QWidget* parent = nullptr);

    void setMessage(const QString& msg);
    void setProgress(int value);     // 0..100
    void setIndeterminate(bool on);  // бесконечный прогресс (busy)

protected:
    bool event(QEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    QWidget* m_panel = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_message = nullptr;
    QProgressBar* m_bar = nullptr;

    void centerPanel();
};

#endif // LOADINGOVERLAY_H
