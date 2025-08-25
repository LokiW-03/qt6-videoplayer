#pragma once

#include <QPointF>
#include <QQuickItem>
#include <QRectF>
#include <QtQml/qqml.h>
#include <memory>
#include "frames/frameData.h"
#include "frames/frameQueue.h"
#include "rendering/diffRenderer.h"
#include "utils/sharedViewProperties.h"

class DiffWindow : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qreal getAspectRatio READ getAspectRatio CONSTANT)
    Q_PROPERTY(qreal maxZoom READ maxZoom WRITE setMaxZoom NOTIFY maxZoomChanged)
    Q_PROPERTY(SharedViewProperties* sharedView READ sharedView WRITE setSharedView NOTIFY sharedViewChanged)
    Q_PROPERTY(int osdState READ osdState WRITE setOsdState NOTIFY osdStateChanged)
    Q_PROPERTY(int currentFrame READ currentFrame NOTIFY currentFrameChanged)
    Q_PROPERTY(int totalFrames READ totalFrames)
    Q_PROPERTY(QString pixelFormat READ pixelFormat)
    Q_PROPERTY(QString timeBase READ timeBase)
    Q_PROPERTY(qint64 duration READ duration)
    Q_PROPERTY(double currentTimeMs READ currentTimeMs NOTIFY currentTimeMsChanged)
    Q_PROPERTY(int displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(float diffMultiplier READ diffMultiplier WRITE setDiffMultiplier NOTIFY diffMultiplierChanged)
    Q_PROPERTY(int diffMethod READ diffMethod WRITE setDiffMethod NOTIFY diffMethodChanged)

  public:
    explicit DiffWindow(QQuickItem* parent = nullptr);
    SharedViewProperties* sharedView() const;
    void setSharedView(SharedViewProperties* view);
    void initialize(std::shared_ptr<FrameMeta> metaPtr,
                    std::shared_ptr<FrameQueue> queuePtr1,
                    std::shared_ptr<FrameQueue> queuePtr2);
    DiffRenderer* m_renderer = nullptr;
    void setAspectRatio(int width, int height);
    qreal getAspectRatio() const;
    qreal maxZoom() const;
    void setMaxZoom(qreal zoom);
    Q_INVOKABLE QVariant getDiffValue(int x, int y) const;

    // OSD-related methods
    int osdState() const { return m_osdState; }
    void setOsdState(int state);
    int currentFrame() const;
    int totalFrames() const;
    QString pixelFormat() const;
    QString timeBase() const;
    qint64 duration() const;
    double currentTimeMs() const;

    // Diff configuration methods
    int displayMode() const { return m_displayMode; }
    void setDisplayMode(int mode);
    float diffMultiplier() const { return m_diffMultiplier; }
    void setDiffMultiplier(float multiplier);
    int diffMethod() const { return m_diffMethod; }
    void setDiffMethod(int method);

    // For test oracles
    QRectF getSelectionRect() const { return m_selectionRect; }
    bool hasSelection() const { return m_hasSelection; }
    QPointF selectionStart() const { return m_selectionStart; }
    QPointF selectionEnd() const { return m_selectionEnd; }
    bool isSelecting() const { return m_isSelecting; }

  public slots:
    void uploadFrame(FrameData* frame1, FrameData* frame2);
    void renderFrame();
    void batchIsFull();
    void batchIsEmpty();
    void rendererError();
    Q_INVOKABLE void zoomAt(qreal factor, const QPointF& centerPoint);
    void setSelectionRect(const QRectF& rect);
    void clearSelection();
    Q_INVOKABLE void zoomToSelection(const QRectF& rect);
    Q_INVOKABLE void resetView();
    Q_INVOKABLE void pan(const QPointF& delta);
    QVariantMap getFrameMeta() const;
    void toggleOsd();
    void updateFrameInfo(int currentFrame, double currentTimeMs);

  signals:
    void batchUploaded(bool success);
    void gpuUploaded(bool success);
    void errorOccurred();
    void selectionChanged(const QRectF& rect);
    void zoomChanged();
    void maxZoomChanged();
    void sharedViewChanged();
    void frameReady();
    void osdStateChanged();
    void currentFrameChanged();
    void currentTimeMsChanged();
    void displayModeChanged();
    void diffMultiplierChanged();
    void diffMethodChanged();

  protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

  private:
    QRectF m_selectionRect;
    bool m_hasSelection = false;
    QPointF m_selectionStart;
    QPointF m_selectionEnd;
    bool m_isSelecting = false;
    qreal m_videoAspectRatio = 16.0 / 9.0;
    qreal m_maxZoom = 10000.0;
    SharedViewProperties* m_sharedView = nullptr;

    // OSD-related members
    int m_osdState = 0; // 0: hidden, 1: basic info, 2: detailed info
    std::shared_ptr<FrameMeta> m_frameMeta;
    std::shared_ptr<FrameQueue> m_frameQueue1;
    std::shared_ptr<FrameQueue> m_frameQueue2;
    int m_currentFrame = 0;
    double m_currentTimeMs = 0.0;

    // Diff configuration members
    int m_displayMode = 0;         // 0=Grayscale Classic, 1=Heatmap, 2=Binary
    float m_diffMultiplier = 4.0f; // diff multiplier
    int m_diffMethod = 0;          // 0=Direct Subtraction, 1=Squared Difference, 2=Normalized, 3=Absolute Difference

    QRectF getVideoRect() const;
    QPointF convertToVideoCoordinates(const QPointF& point) const;
};