/**
*	@file:	QssTtitleBar.h 
*	@details:	css�ؼ�����Դ�ļ�����������UI�ؼ����Զ���������������ڣ��Ի�����Ϣ��
*	@author: 	chenwen(chenwen1126@tom.com)
*   @datetime:  2017-7-25
*   @history:   v1.0 first edition(Qt4.7)
*/
#include "Qss.h"

#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QtMath>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>
#include <QFile>

#define QSSDIALOG_SHADOW_WIDTH 12 //ע����QFrame#dialog,QFrame#messagebox ��padding����ֵ���
#define QSSDIALOG_BODER_WIDTH 0

QssTtitleBar::QssTtitleBar(QWidget *parent , QTitleBar_Type type/* = QTitleBar_Type_Window*/)
	: QWidget(parent),m_maxOrRestore(false),m_pressed(false),m_type(type)
{
	setObjectName("titlebar");

	m_closeBtn = new QPushButton(this);//�رհ�ť
	m_closeBtn->setObjectName("titlebarclosebtn");//���ÿؼ����ƣ�����cssָ����ʽ
	m_closeBtn->setToolTip(QString::fromLocal8Bit("�ر�"));
	m_closeBtn->setVisible(m_type & QTitleBar_Button_Close);

	m_minBtn = new QPushButton(this);//��С����ť
	m_minBtn->setObjectName("titlebarminbtn");//���ÿؼ����ƣ�����cssָ����ʽ
	m_minBtn->setToolTip(QString::fromLocal8Bit("��С��"));
	m_minBtn->setVisible(m_type & QTitleBar_Button_Min);

	m_restoreBtn = new QPushButton(this);//��󻯰�ť�������ʱ����
	m_restoreBtn->setObjectName("titlebarrestorebtn");//���ÿؼ����ƣ�����cssָ����ʽ
	m_restoreBtn->setToolTip(QString::fromLocal8Bit("��ԭ"));
	m_restoreBtn->setVisible(m_type & QTitleBar_Button_Restore);

	m_maxBtn = new QPushButton(this);//�ָ���ť����normalʱ����
	m_maxBtn->setObjectName("titlebarmaxbtn");//���ÿؼ����ƣ�����cssָ����ʽ
	m_maxBtn->setToolTip(QString::fromLocal8Bit("���"));
	m_maxBtn->setVisible(m_type & QTitleBar_Button_Max);

	m_iconBtn = new QPushButton(this);//������ͼ��
	m_iconBtn->setObjectName("titlebaricon");//���ÿؼ����ƣ�����cssָ����ʽ

	m_titlebarTitle = new QLabel(this);//����������
	m_titlebarTitle->setObjectName("titlebartitle");//���ÿؼ����ƣ�����cssָ����ʽ

	/** �������Ԫ�ؼ���һ��ˮƽ���� */
	QHBoxLayout* hBox = new QHBoxLayout(this);
	hBox->setMargin(0);
	hBox->addWidget(m_iconBtn);
	hBox->addWidget(m_titlebarTitle);
	hBox->addStretch(1);//ˮƽռλ
	hBox->addWidget(m_minBtn);
	hBox->addWidget(m_restoreBtn);
	m_restoreBtn->setVisible(m_maxOrRestore);//Ĭ������
	hBox->addWidget(m_maxBtn);
	hBox->addWidget(m_closeBtn);

	hBox->setSpacing(0);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//���ñ������ߴ���ԣ�ˮƽΪ��չ����ֱΪ�̶�

	connect(m_closeBtn, SIGNAL(clicked()), parent, SLOT(close()));//�رհ�ť����ʱ�������ڹر�
	connect(m_minBtn, SIGNAL(clicked()), parent, SLOT(showMinimized()));
	connect(m_maxBtn, SIGNAL(clicked()), this, SLOT(onMaxOrRestore()));//��󻯺�������ʾ
	connect(m_restoreBtn, SIGNAL(clicked()), this, SLOT(onMaxOrRestore()));

	m_iconBtn->installEventFilter(this);//��װ�¼����ˣ���ȡm_iconLab�ϵ�˫��ʱ�䣬�رմ���
	installEventFilter(this);//��װ�¼����ˣ���ȡ�������ϵ�˫���¼�����󻯺�������ʾ��ʾ����

	m_rcValid = QApplication::desktop()->availableGeometry(); 
}

QssTtitleBar::~QssTtitleBar()
{
	//qDebug("=========QTitleBar-0x%x:deleteed!!========", this);
}

void QssTtitleBar::setTitle( QString title )
{
	m_titlebarTitle->setText(title);
}

void QssTtitleBar::setIcon( QIcon icon)
{
	m_iconBtn->setIcon(icon);
}

void QssTtitleBar::setMaxOrRestore( bool val)
{
	m_maxOrRestore = val;//true�����

	if ((m_type & QTitleBar_Button_Restore) && (m_type & QTitleBar_Button_Max))
	{
		m_restoreBtn->setVisible(m_maxOrRestore);
		m_maxBtn->setVisible(!m_maxOrRestore);
	}
}

/** m_maxNormalΪtrue��ʾ�����������ʾ */
void QssTtitleBar::onMaxOrRestore()
{
	if (m_type != QTitleBar_Type_MainWindow)
		return ;
	
	if (m_maxOrRestore)
	{
		if (!m_rcNormal.isValid())
		{
			QSize sizeHint = parentWidget()->sizeHint();
			if (sizeHint.width() > m_rcValid.width())
				sizeHint.setWidth(m_rcValid.width() -100);
			if (sizeHint.height() > m_rcValid.height())
				sizeHint.setHeight(m_rcValid.height() - 100);

			m_rcNormal.setLeft((m_rcValid.width() - sizeHint.width())/2);
			m_rcNormal.setTop((m_rcValid.height() - sizeHint.height())/2);
			m_rcNormal.setWidth(sizeHint.width());
			m_rcNormal.setHeight(sizeHint.height());
		}
		parentWidget()->setGeometry(m_rcNormal);
	}
	else 
	{
		/** parentWidget()��QWindowFrameȥ����������������ʾʱ����ȫ����ʾ
		����ֻ�����������������ʾʱ����ʾ�ߴ�δ������Ч��������Ҳ�����˴���
		��ʾ״̬�Ĳ�׼ȷ�ԣ����´���ϵͳ�˵�����������������ϵͳ�˵�����������
		�������رղ˵�����⣩��״̬��������С�����Ǳ겻����ȷ��ʵ�ı׶ˣ���ʱû�н���취����*/
		QRect rc = parentWidget()->geometry();
		if (rc.width() < m_rcValid.width() && rc.height() < m_rcValid.height())
  			m_rcNormal = rc;
		parentWidget()->setGeometry(m_rcValid);
	}

	setMaxOrRestore(!m_maxOrRestore);
}

/** ��Ҫͨ��QWidget#�����ñ�����css��ʽ����Ҫ����QWidget CSS��ʽ���������¹̶���ʽ�Ĵ��롣
����Ĵ�����û��ָ��QWidget CSS��ʽʱ����Ҳ���� */
void QssTtitleBar::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/** mouseMoveEvent mousePressEvent������������Ϊ��ʵ���ڱ������϶����� 
ע��mouseMoveEvent�¼���δ����setMouseTrackingʱ��ֻ�а�ס����ʱ�Żᷢ��*/
void QssTtitleBar::mouseMoveEvent( QMouseEvent * ev )
{
	if (m_maxOrRestore)
		return;
	if (!m_pressed)
		return;

	QPoint globalPt = ev->globalPos();
	QPoint movePt = globalPt - m_pressedPos;//����Frame�������Ͻǵ�globalPos
	parentWidget()->move(movePt);//�ƶ���ָ��globalPos

	return ;
}

void QssTtitleBar::mousePressEvent( QMouseEvent * ev )
{
	m_pressed = true;
	m_pressedPos = mapToParent(ev->pos());

	return ;
}

void QssTtitleBar::mouseReleaseEvent( QMouseEvent * ev )
{
	m_pressed = false;
}

/** ��;��ȡ������˫����Ϣ�ͱ�����ͼ��˫����Ϣ���ֱ����ڴ��������С���л���˫��������ͼ��رմ��� */
bool QssTtitleBar::eventFilter( QObject * obj, QEvent * ev )
{
	if (obj == this)
	{
		if (ev->type() == QEvent::MouseButtonDblClick)
		{
			onMaxOrRestore();
			return true;
		}
	}
	else if (obj == m_iconBtn)
	{
		if (ev->type() == QEvent::MouseButtonDblClick)
		{
			parentWidget()->close();
			return true;
		}
	}

	return QWidget::eventFilter(obj, ev);
}

/** �Զ��������ͱ߿�Ĵ��ڵĲ����ǣ���һ��QWindowFrame�в���ֱ�����ٷֱ�����Զ��������ʹ������� */
QssMainWindow::QssMainWindow(QWidget *parent/* = 0*/, Qt::WindowFlags flags/* = 0*/)
: QMainWindow(parent, flags),m_mousePressedInBoundy(false)
{
	m_rcValid = QApplication::desktop()->availableGeometry(); 
	
	m_frame = new QFrame(parent, flags);
	//���ÿؼ����ƣ�����cssָ����ʽ
	m_frame->setObjectName("window");
	/** frame����Ϊ�޿�ܣ�QFrame�޸��ؼ�ʱ�п�ܣ������ر������ͱ߿��Ա��Զ���������ͱ߿� 
	Qt::WindowSystemMenuHint ָ��frame��ϵͳ�˵���ֻ����ϵͳ�˵���ֻ����С���͹رտ���*/
	//WindowMinimizeButtonHintʹ��ϵͳ�˵�����С���ͻ�ԭ�˵�����ã������û�ԭ�˵���Ŀʱ��ԭ���ڳߴ粻�ԣ�
	//��Ϊ���д��ڳߴ�ʱ�����Զ����m_rcNormal��ϵͳ����֪��������ֱ�ӻ�ԭ�����ߴ硣
	m_frame->setWindowFlags(Qt::Window | Qt::FramelessWindowHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
	//��꾭���ؼ�ʱ��mouseMoveEvent,����ֻ����갴��ͬʱ����mouseMoveEvent�¼���MouseTrackingĬ��Ϊfalse
	//���￪MouseTracking��ҪΪ����꾭���߿�ʱ�����ָ���Ϊ�ɵ�����С�����ָ����ʽ
	m_frame->setMouseTracking(true);
	m_frame->installEventFilter(this);
	
	m_titleBar = new QssTtitleBar(m_frame);
	m_titleBar->installEventFilter(this);

	/** QFrame�Ĵ�ֱ�����м����������ȥ���˱������ͱ߿�Ĵ������ݣ�����QFrame�ͳ������ǿ������Զ��˱������ͱ߿�Ĵ��� */
	QVBoxLayout* vbox = new QVBoxLayout(m_frame);
	vbox->setMargin(0);
	vbox->setSpacing(0);
	vbox->addWidget(m_titleBar);
	vbox->addWidget(this);

 	installEventFilter(this);

	/** ����Ч�� */

}

QssMainWindow::~QssMainWindow()
{
	m_frame->deleteLater();
}

void QssMainWindow::show()
{
	m_frame->show();
}

void QssMainWindow::showMinimized()
{
	m_frame->showMinimized();
}

void QssMainWindow::showMaximized()
{
	m_titleBar->setMaxOrRestore(true);
	m_frame->setGeometry(m_rcValid);
}

void QssMainWindow::showFullScreen()
{
	m_titleBar->setMaxOrRestore(true);
	m_frame->showFullScreen();
}

void QssMainWindow::showNormal()
{
	m_titleBar->setMaxOrRestore(false);
	m_frame->resize(rect().width(), rect().height() + m_titleBar->rect().height());
	m_frame->showNormal();
}

void QssMainWindow::setWindowTitle( QString title )
{
	m_frame->setWindowTitle(title);
	m_titleBar->setTitle(title);
}

void QssMainWindow::setWindowIcon( QIcon icon )
{
	m_frame->setWindowIcon(icon);
	m_titleBar->setIcon(icon);
}

void QssMainWindow::onMouseMoveEvent( QMouseEvent * ev )
{
	if (m_titleBar->maxOrRestore())
	{
		return;
	}

	if (m_mousePressedInBoundy)
	{
		int x = ev->globalPos().x();
		int y = ev->globalPos().y();

		int dx = x - m_pos.x();
		int dy = y - m_pos.y();

		if ((m_left || m_right) && qAbs(dx) < 5)
			return;
		if ((m_top || m_bottom) && qAbs(dy) < 5)
			return;
		if (m_left && dx > 0 && m_rect.width() <= m_frame->minimumWidth())
			return ;
		if (m_top && dy > 0 && m_rect.height() <= m_frame->minimumHeight())
			return;

		QRect rc = m_rect;
		if (m_left)
			rc.setLeft(rc.left() + dx);
		if (m_right)
			rc.setRight(rc.right() + dx);
		if (m_top)
			rc.setTop(rc.top() + dy);
		if (m_bottom)
			rc.setBottom(rc.bottom() + dy);

		m_frame->setGeometry(rc);
		m_rect = rc;
		m_pos = ev->globalPos();
	}
	else
	{
		int x = ev->x();
		int y = ev->y();

		QRect rc = m_frame->rect();
		m_left = qAbs(x - rc.left()) <= 5;
		m_right = qAbs(x - rc.right()) <= 5;
		m_top = qAbs(y - rc.top()) <= 5;
		m_bottom = qAbs(y - rc.bottom()) <= 5;

		if ((m_left && m_top) || (m_right && m_bottom))
			m_frame->setCursor(Qt::SizeFDiagCursor);
		else if ((m_right && m_top) || (m_left && m_bottom))
			m_frame->setCursor(Qt::SizeBDiagCursor);
		else if ((m_left && !m_top && !m_bottom) || (m_right && !m_top && !m_bottom))
			m_frame->setCursor(Qt::SizeHorCursor);
		else if ((m_top && !m_right && !m_left) || (m_bottom && !m_right && !m_left))
			m_frame->setCursor(Qt::SizeVerCursor);
		else
			m_frame->setCursor(Qt::ArrowCursor);
	}
}

void QssMainWindow::onMousePressEvent( QMouseEvent * ev )
{
	m_pos = ev->globalPos();
	m_rect = m_frame->geometry();
	m_mousePressedInBoundy = (ev->button() == Qt::LeftButton) && (m_left || m_right || m_top || m_bottom);
}

void QssMainWindow::onMouseReleaseEvent( QMouseEvent * ev )
{
	m_mousePressedInBoundy = false;
}

bool QssMainWindow::eventFilter( QObject * obj, QEvent * ev )
{
	if (obj == m_frame)
	{
		if (ev->type() == QEvent::MouseMove)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMouseMoveEvent(mouseEv);
				return true;
			}
		}
		else if (ev->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMousePressEvent(mouseEv);
				return true;
			}
		}
		else if (ev->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMouseReleaseEvent(mouseEv);
				return true;
			}
		}
		else if (ev->type() == QEvent::Show)
		{
			/* ��ʾʱ��Ļ������ʾ */
			QRect rc = m_frame->rect();
			QRect parentRc = m_rcValid;

 			/** m_frame�����޸��ؼ��ģ�����Ҫ�����Ļ�����ƶ������겻��Ϊ�� */
 			int x = parentRc.left() + (parentRc.width() - rc.width())*0.5; x = x <= 0 ? 1 : x;
 			int y = parentRc.top() + (parentRc.height() - rc.height())*0.5; y = y <= 0 ? 1 : y;
 			m_frame->move(x, y);

			//��QssMainWindow����ʹ����ʾ���Զ�������Ϊʹ�����Զ����ᵼ��QssMainWindow����������ʾΪ��С�ߴ�
		}
 		else if (ev->type() == QEvent::Close)
 		{
 			close();
 			m_titleBar->close();

			return true;
 		}
	}
	else if (obj == m_titleBar)
	{
		if (ev->type() == QEvent::Enter)
		{
			m_left = false;m_right = false; m_top = false; m_bottom = false;
			if (m_frame->cursor().shape() != Qt::ArrowCursor)
				m_frame->setCursor(Qt::ArrowCursor);
		}
	}
	else if (obj == this)
	{
		if (ev->type() == QEvent::Enter)
		{
			m_left = false;m_right = false; m_top = false; m_bottom = false;
			if (m_frame->cursor().shape() != Qt::ArrowCursor)
				m_frame->setCursor(Qt::ArrowCursor);
		}
	}

	return QMainWindow::eventFilter(obj, ev);
}

QssDialog::QssDialog(QWidget *parent)
	: QDialog(0),
	m_mousePressedInBorder(false),
	m_parent(parent)
{
	m_rcValid = QApplication::desktop()->availableGeometry(); 

	m_frame = new QFrame(parent);
	m_frame->setObjectName("dialog");//���ÿؼ����ƣ�����cssָ����ʽ
	m_frame->setAttribute(Qt::WA_TranslucentBackground);/** ������������͸��������padding����Ӱ�߿���ƿؼ� ��ע�����ø����Ժ�css boder��ʧЧ��������Ӱʱͬʱ���Ʊ߿�*/
	m_frame->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint| Qt::WindowSystemMenuHint/* | Qt::WindowMinimizeButtonHint*/);//����Qt::WindowMinimizeButtonHint���Ժ�dialogͨ����������С���ͻ�ԭ�󣬵����߿������⣬����������ʱ�����ϸ�����
	m_frame->setMouseTracking(true);
	m_frame->installEventFilter(this);

	m_titleBar = new QssTtitleBar(m_frame, QssTtitleBar::QTitleBar_Type_Dialog);
	m_titleBar->installEventFilter(this);

	QVBoxLayout* vbox = new QVBoxLayout(m_frame);
	vbox->setMargin(0);
	vbox->setSpacing(0);
	vbox->addWidget(m_titleBar);
	vbox->addWidget(this);

	installEventFilter(this);
}

QssDialog::~QssDialog()
{
	m_frame->deleteLater();
}

void QssDialog::show()
{
	/** ���ݱ�������������ʵ�ߴ�resize m_frame������m_frame����ʾsizehint�ߴ� */
	int offset = (QSSDIALOG_SHADOW_WIDTH + QSSDIALOG_BODER_WIDTH)*2;//rect()��óߴ��Ϊ�������ߴ磬�������߿��padding�ߴ硣����Ҫ���ϱ߿��padding�ߴ�ƫ�Ʋ��ܻ�ȡ��ȷ��frame�ߴ�
	m_frame->resize(rect().width() + offset, rect().height() + m_titleBar->rect().height() + offset);

	QDialog::show();
	m_frame->show();
}

void QssDialog::raise()
{
	m_frame->raise();
}

void QssDialog::activateWindow()
{
	m_frame->activateWindow();
}

int QssDialog::exec()
{
	/** ���ݱ�������������ʵ�ߴ�resize m_frame������m_frame����ʾsizehint�ߴ� */
	int offset = (QSSDIALOG_SHADOW_WIDTH + QSSDIALOG_BODER_WIDTH)*2;//rect()��óߴ��Ϊ�������ߴ磬�������߿��padding�ߴ硣����Ҫ���ϱ߿��padding�ߴ�ƫ�Ʋ��ܻ�ȡ��ȷ��frame�ߴ�
	m_frame->resize(rect().width() + offset, rect().height() + m_titleBar->rect().height() + offset);

	m_frame->setWindowModality(Qt::ApplicationModal);//����ΪQt::ApplicationModal�����򵯳���ģ̬�Ի��������������ʾ
	//m_frame->setWindowFlags(m_frame->windowFlags() | Qt::Tool);//���Լ���Qt::Tool��־ʹ�䲻��������������

	m_frame->show();
	m_frame->raise();
	m_frame->activateWindow();

	int ret = QDialog::exec();
	m_frame->close();

	return ret;
}

void QssDialog::setWindowTitle( QString title )
{
	m_frame->setWindowTitle(title);
	m_titleBar->setTitle(title);
}

void QssDialog::setWindowIcon( QIcon icon )
{
	m_frame->setWindowIcon(icon);
	m_titleBar->setIcon(icon);
}

/** dialog������Ӱ�߿���ܵ��ڳߴ��С */
void QssDialog::onMouseMoveEvent( QMouseEvent * ev )
{
	if (m_mousePressedInBorder)
	{
		int x = ev->globalPos().x();
		int y = ev->globalPos().y();

		int dx = x - m_pos.x();
		int dy = y - m_pos.y();

		if ((m_left || m_right) && qAbs(dx) < 5)
			return;
		if ((m_top || m_bottom) && qAbs(dy) < 5)
			return;
		if (m_left && dx > 0 && m_rect.width() <= m_frame->minimumWidth())
			return ;
		if (m_top && dy > 0 && m_rect.height() <= m_frame->minimumHeight())
			return;

		QRect rc = m_rect;
		if (m_left)
			rc.setLeft(rc.left() + dx);
		if (m_right)
			rc.setRight(rc.right() + dx);
		if (m_top)
			rc.setTop(rc.top() + dy);
		if (m_bottom)
			rc.setBottom(rc.bottom() + dy);

		m_frame->setGeometry(rc);
		m_rect = rc;
		m_pos = ev->globalPos();
	}
	else
	{
		int x = ev->x() + QSSDIALOG_SHADOW_WIDTH - 2;
		int y = ev->y() + QSSDIALOG_SHADOW_WIDTH - 2;
		
		QRect rc = m_frame->rect();
		m_left = qAbs(x - rc.left()) <= 5;
		m_right = qAbs(x - rc.right()) <= 5;
		m_top = qAbs(y - rc.top()) <= 5;
		m_bottom = qAbs(y - rc.bottom()) <= 5;

		if ((m_left && m_top) || (m_right && m_bottom))
			m_frame->setCursor(Qt::SizeFDiagCursor);
		else if ((m_right && m_top) || (m_left && m_bottom))
			m_frame->setCursor(Qt::SizeBDiagCursor);
		else if ((m_left && !m_top && !m_bottom) || (m_right && !m_top && !m_bottom))
			m_frame->setCursor(Qt::SizeHorCursor);
		else if ((m_top && !m_right && !m_left) || (m_bottom && !m_right && !m_left))
			m_frame->setCursor(Qt::SizeVerCursor);
		else
			m_frame->setCursor(Qt::ArrowCursor);
	}
}

void QssDialog::onMousePressEvent( QMouseEvent * ev )
{
	m_pos = ev->globalPos();
	m_rect = m_frame->geometry();
	if(m_left || m_right || m_top || m_bottom)
	{
		m_mousePressedInBorder = ev->button() == Qt::LeftButton;
		//qDebug() << "mousePressed pressed in border";
	}
}

void QssDialog::onMouseReleaseEvent( QMouseEvent * ev )
{
	m_mousePressedInBorder = false;
	//qDebug() << "mousePressed release in border";
}

bool QssDialog::eventFilter( QObject * obj, QEvent * ev )
{
	if (obj == m_frame)
	{
		if (ev->type() == QEvent::MouseMove)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMouseMoveEvent(mouseEv);
				return true;
			}
		}
		else if (ev->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMousePressEvent(mouseEv);
				return true;
			}
		}
		else if (ev->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent * mouseEv = dynamic_cast<QMouseEvent *>(ev);
			if (ev)
			{
				onMouseReleaseEvent(mouseEv);
				return true;
			}
		}/** ������Ӱ�߿� */
		else if (ev->type() == QEvent::Paint)
		{
			const int shadowWidth = QSSDIALOG_SHADOW_WIDTH;
			const int boderWidht = QSSDIALOG_BODER_WIDTH;

			QPainter paiter(m_frame);

			QColor colorBorder(0xaa,0xaa,0xaa);
			paiter.setPen(colorBorder);

			int boderOffset = shadowWidth + boderWidht - 1;
			paiter.drawLine(boderOffset,boderOffset,m_frame->width() - boderOffset - 1, boderOffset);//top
			paiter.drawLine(boderOffset,m_frame->height() - boderOffset - 1,m_frame->width() - boderOffset - 1, m_frame->height() - boderOffset - 1);//bottom

			paiter.drawLine(boderOffset,boderOffset,boderOffset,m_frame->height() - boderOffset - 1);//left
			paiter.drawLine(m_frame->width() - boderOffset - 1,boderOffset,m_frame->width() - boderOffset - 1,m_frame->height() - boderOffset - 1);//right

			QColor colorShadow(0xaa,0xaa,0xaa);
			for (int i = 0; i < shadowWidth; i++)
			{
				colorShadow.setAlpha(100*cos(1.5707963*i/(shadowWidth - 1)));
				paiter.setPen(colorShadow);
				paiter.drawRect(boderOffset + i, boderOffset + i, 
					m_frame->width() - 2*shadowWidth , 
					m_frame->height() - 2*shadowWidth );
			}
		}
		else if (ev->type() == QEvent::Show)
		{
			QRect rc = m_frame->rect(), parentRc;
			if (m_parent)/**�и��ؼ�ʱ�Ӹ��ؼ��� */
			{
				/** ���ؼ�����תΪ��Ļ���� */
				QPoint pt = m_parent->mapToGlobal(QPoint(0,0));
				parentRc =m_parent->rect();
				parentRc.translate(pt);
			}
			else/**�޸��ؼ�ʱ����Ļ�� */
				parentRc = m_rcValid;

			/** m_frame�����޸��ؼ��ģ�����Ҫ�����Ļ�����ƶ������겻��Ϊ�� */
			int x = parentRc.left() + (parentRc.width() - rc.width())*0.5;x = x <= 0?1:x;
			int y = parentRc.top() + (parentRc.height() - rc.height())*0.5;y = y <= 0?1:y;
			m_frame->move(x,y);
			
 			/** ���Զ��� */
 			QPropertyAnimation* aniSize = new QPropertyAnimation(m_frame,"geometry");
 			aniSize->setDuration(200);
 			aniSize->setKeyValueAt(0, QRect(x,y,0,0));
 			aniSize->setKeyValueAt(0.5, QRect(x ,y,rc.width() + 20,rc.height() + 30));
 			aniSize->setKeyValueAt(1 , QRect(x,y,rc.width(),rc.height()));
 			//aniSize->setEasingCurve(QEasingCurve::InOutBack);//�������߻�������̫����
 
 			QPropertyAnimation* aniOpacity = new QPropertyAnimation(m_frame,"windowOpacity");
 			aniOpacity->setDuration(200);
 			aniOpacity->setStartValue(0);
 			aniOpacity->setEndValue(1);
 
 			QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(m_frame);
 			aniGroup->addAnimation(aniSize);
 			aniGroup->addAnimation(aniOpacity);
 			
 			/** ��ʾ������ʼ���ú���������������������ɺ�ɾ�� */
 			aniGroup->start(QAbstractAnimation::DeleteWhenStopped);

		}
		else if (ev->type() == QEvent::Close)
		{
			close();
		}
	}
	else if (obj == m_titleBar)
	{
		if (ev->type() == QEvent::Enter)
		{
			m_left = false;m_right = false; m_top = false; m_bottom = false;
			if (m_frame->cursor().shape() != Qt::ArrowCursor)
				m_frame->setCursor(Qt::ArrowCursor);
		}
	}
	else if (obj == this)
	{
		if (ev->type() == QEvent::Enter)
		{
			m_left = false;m_right = false; m_top = false; m_bottom = false;
			if (m_frame->cursor().shape() != Qt::ArrowCursor)
				m_frame->setCursor(Qt::ArrowCursor);
		}
		else if (ev->type() == QEvent::Hide)
		{
			m_frame->hide();
			return true;
		}
	}

	return QDialog::eventFilter(obj, ev);
}

QssMessageBox::QssMessageBox( Icon icon, const QString &title, const QString &text, StandardButtons buttons /*= NoButton*/, QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= Qt::Widget | Qt::FramelessWindowHint*/ )
:QMessageBox(icon, title, text, buttons, 0, flags),m_parent(parent)
{
	m_rcValid = QApplication::desktop()->availableGeometry(); 

	m_frame = new QFrame;
	m_frame->setObjectName("messagebox");//���ÿؼ����ƣ�����cssָ����ʽ
	m_frame->setAttribute(Qt::WA_TranslucentBackground);/** ����͸��������padding����Ӱ�߿���ƿؼ� */
	m_frame->setWindowFlags(Qt::Dialog |Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
	m_frame->setMouseTracking(true);
	m_frame->installEventFilter(this);

	m_frame->setWindowTitle(title);
	m_frame->setWindowIcon(style()->standardIcon((QStyle::StandardPixmap)(icon + 8)));
	m_frame->setWindowModality(Qt::ApplicationModal);

	m_titleBar = new QssTtitleBar(m_frame, QssTtitleBar::QTitleBar_Type_MessageBox);
	m_titleBar->installEventFilter(this);

	m_titleBar->setTitle(title);
	m_titleBar->setIcon(style()->standardIcon((QStyle::StandardPixmap)(icon + 8)));

	QVBoxLayout* vbox = new QVBoxLayout(m_frame);
	vbox->setMargin(0);
	vbox->setSpacing(0);
	vbox->addWidget(m_titleBar);
	vbox->addWidget(this);

	installEventFilter(this);
}

QssMessageBox::QssMessageBox( QWidget *parent /*= 0*/ )
:QMessageBox(parent),m_parent(parent)
{
	m_rcValid = QApplication::desktop()->availableGeometry(); 

	m_frame = new QFrame;
	m_frame->setObjectName("messagebox");//���ÿؼ����ƣ�����cssָ����ʽ
	m_frame->setAttribute(Qt::WA_TranslucentBackground);/** ����͸��������padding����Ӱ�߿���ƿؼ� */
	m_frame->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
	m_frame->setMouseTracking(true);
	m_frame->installEventFilter(this);
	m_frame->setWindowModality(Qt::ApplicationModal);

	m_titleBar = new QssTtitleBar(m_frame, QssTtitleBar::QTitleBar_Type_MessageBox);
	m_titleBar->installEventFilter(this);

	QVBoxLayout* vbox = new QVBoxLayout(m_frame);
	vbox->setMargin(0);
	vbox->setSpacing(0);
	vbox->addWidget(m_titleBar);
	vbox->addWidget(this);

	installEventFilter(this);
}

QssMessageBox::~QssMessageBox()
{
	m_frame->deleteLater();
}

bool QssMessageBox::eventFilter( QObject * obj, QEvent * ev )
{
	if (obj == m_frame)
	{
		if (ev->type() == QEvent::Paint)
		{
			const int shadowWidth = QSSDIALOG_SHADOW_WIDTH;
			const int boderWidht = QSSDIALOG_BODER_WIDTH;

			QPainter paiter(m_frame);

			QColor colorBorder(0xaa,0xaa,0xaa);
			paiter.setPen(colorBorder);

			int boderOffset = shadowWidth + boderWidht - 1;
			paiter.drawLine(boderOffset,boderOffset,m_frame->width() - boderOffset - 1, boderOffset);//top
			paiter.drawLine(boderOffset,m_frame->height() - boderOffset - 1,m_frame->width() - boderOffset - 1, m_frame->height() - boderOffset - 1);//bottom

			paiter.drawLine(boderOffset,boderOffset,boderOffset,m_frame->height() - boderOffset - 1);//left
			paiter.drawLine(m_frame->width() - boderOffset - 1,boderOffset,m_frame->width() - boderOffset - 1,m_frame->height() - boderOffset - 1);//right

			QColor colorShadow(0xaa,0xaa,0xaa);
			for (int i = 0; i < shadowWidth; i++)
			{
				colorShadow.setAlpha(100*cos(1.5707963*i/(shadowWidth - 1)));
				paiter.setPen(colorShadow);
				paiter.drawRect(boderOffset + i, boderOffset + i, 
					m_frame->width() - 2*shadowWidth , 
					m_frame->height() - 2*shadowWidth );
			}
		}
		else if (ev->type() == QEvent::Show)
		{
			QRect rc = m_frame->rect(), parentRc;
			if (m_parent)/**�и��ؼ�ʱ�Ӹ��ؼ��� */
			{
				QPoint pt = m_parent->mapToGlobal(QPoint(0,0));
				parentRc =m_parent->rect();
				parentRc.translate(pt);
			}
			else/**�޸��ؼ�ʱ����Ļ�� */
				parentRc = m_rcValid;
			/** m_frame�����޸��ؼ��ģ�����Ҫ�����Ļ�����ƶ������겻��Ϊ�� */
			int x = parentRc.left() + (parentRc.width() - rc.width())*0.5;x = x <= 0?1:x;
			int y = parentRc.top() + (parentRc.height() - rc.height())*0.5;y = y <= 0?1:y;
			m_frame->move(x,y);

			/** ���Զ��� */
			QPropertyAnimation* aniSize = new QPropertyAnimation(m_frame,"geometry");
			aniSize->setDuration(200);
			aniSize->setKeyValueAt(0, QRect(x,y,0,0));
			aniSize->setKeyValueAt(0.5, QRect(x ,y,rc.width() + 10,rc.height() + 15));
			aniSize->setKeyValueAt(1 , QRect(x,y,rc.width(),rc.height()));
			//aniSize->setEasingCurve(QEasingCurve::InOutBack);//�������߻�������̫����

			QPropertyAnimation* aniOpacity = new QPropertyAnimation(m_frame,"windowOpacity");
			aniOpacity->setDuration(200);
			aniOpacity->setStartValue(0);
			aniOpacity->setEndValue(1);

			QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(m_frame);
			aniGroup->addAnimation(aniSize);
			aniGroup->addAnimation(aniOpacity);

			/** ��ʾ������ʼ���ú���������������������ɺ�ɾ�� */
			aniGroup->start(QAbstractAnimation::DeleteWhenStopped);

		}
		else if (ev->type() == QEvent::Close)
		{
			close();
			m_titleBar->close();
		}
	}
	else if(obj == this)
	{
		if (ev->type() == QEvent::Resize)
		{
			/* ���QMessageBox�Զ��������ݵ������ʱ��frameδ������ȵ���frame���ֱ���͸����bug */
			frame()->setFixedWidth(size().width() + 2*QSSDIALOG_SHADOW_WIDTH);
		}
	}

	return QDialog::eventFilter(obj, ev);
}

QMessageBox::StandardButton QssMessageBox::tips( const QString & text, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("��ʾ")*/, StandardButtons btn /*= QMessageBox::Ok*/ )
{
	QssMessageBox box(QMessageBox::Information, title, "\n" + text/* + "\n"*/, btn, parent);
	box.setDefaultButton(QMessageBox::Ok);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}

QMessageBox::StandardButton QssMessageBox::warn( const QString & text, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("����")*/, StandardButtons btn /*= QMessageBox::Ok*/ )
{
	QssMessageBox box(QMessageBox::Warning, title, "\n" + text/* + "\n"*/ , btn, parent);
	box.setDefaultButton(QMessageBox::Ok);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}

QMessageBox::StandardButton QssMessageBox::error( const QString & text, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("����")*/, StandardButtons btn /*= QMessageBox::Ok*/ )
{
	QssMessageBox box(QMessageBox::Critical, title, "\n" + text/* + "\n"*/, btn, parent);
	box.setDefaultButton(QMessageBox::Ok);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}

QMessageBox::StandardButton QssMessageBox::ask( const QString & text, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("ѯ��")*/, StandardButtons btn /*= QMessageBox::Yes | QMessageBox::No*/ )
{
	QssMessageBox box(QMessageBox::Question, title, "\n" + text/* + "\n"*/, btn, parent);
	box.setDefaultButton(QMessageBox::Yes);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}


QMessageBox::StandardButton QssMessageBox::regard( const QString & text, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("����")*/ )
{
	QssMessageBox box(parent);

	QIcon icon = QApplication::windowIcon();
	QSize size = icon.actualSize(QSize(64, 64));
	box.setIconPixmap(icon.pixmap(size));
	box.setDefaultButton(QMessageBox::Ok);

	box.frame()->setWindowIcon(icon);
	box.titleBar()->setIcon(icon);

	box.frame()->setWindowTitle(title);
	box.titleBar()->setTitle(title);

	box.setInformativeText(text);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}

QMessageBox::StandardButton QssMessageBox::regard( const QString & text, QIcon icon, QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("����")*/ )
{
	QssMessageBox box(parent);
	QSize size = icon.actualSize(QSize(64, 64));
	box.setIconPixmap(icon.pixmap(size));
	box.setDefaultButton(QMessageBox::Ok);
	
	box.frame()->setWindowIcon(icon);
	box.titleBar()->setIcon(icon);

	box.frame()->setWindowTitle(title);
	box.titleBar()->setTitle(title);

	box.setInformativeText(text);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}

QMessageBox::StandardButton QssMessageBox::regardQt(QWidget* parent /*= 0*/, const QString & title /*= QString::fromLocal8Bit("����Qt")*/ )
{
	QString translatedTextAboutQtCaption;
	translatedTextAboutQtCaption = QMessageBox::tr(
		"<h3>About Qt</h3>"
		"<p>This program uses Qt version %1.</p>"
		).arg(QLatin1String(QT_VERSION_STR));
	QString translatedTextAboutQtText;
	translatedTextAboutQtText = QMessageBox::tr(
		"<p>Qt is a C++ toolkit for cross-platform application "
		"development.</p>"
		"<p>Qt provides single-source portability across MS&nbsp;Windows, "
		"Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix variants. "
		"Qt is also available for embedded devices as Qt for Embedded Linux "
		"and Qt for Windows CE.</p>"
		"<p>Qt is available under three different licensing options designed "
		"to accommodate the needs of our various users.</p>"
		"<p>Qt licensed under our commercial license agreement is appropriate "
		"for development of proprietary/commercial software where you do not "
		"want to share any source code with third parties or otherwise cannot "
		"comply with the terms of the GNU LGPL version 2.1 or GNU GPL version "
		"3.0.</p>"
		"<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
		"development of Qt applications (proprietary or open source) provided "
		"you can comply with the terms and conditions of the GNU LGPL version "
		"2.1.</p>"
		"<p>Qt licensed under the GNU General Public License version 3.0 is "
		"appropriate for the development of Qt applications where you wish to "
		"use such applications in combination with software subject to the "
		"terms of the GNU GPL version 3.0 or where you are otherwise willing "
		"to comply with the terms of the GNU GPL version 3.0.</p>"
		"<p>Please see <a href=\"http://qt.nokia.com/products/licensing\">qt.nokia.com/products/licensing</a> "
		"for an overview of Qt licensing.</p>"
		"<p>Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).</p>"
		"<p>Qt is a Nokia product. See <a href=\"http://qt.nokia.com/\">qt.nokia.com</a> "
		"for more information.</p>"
		);

	QPixmap pm(QMessageBox::tr(":/trolltech/qmessagebox/images/qtlogo-64.png"));

	QssMessageBox box(parent);
	box.setWindowTitle(title.isEmpty() ? QString::fromLocal8Bit("����Qt") : title);
	box.setText(translatedTextAboutQtCaption);
	box.setInformativeText(translatedTextAboutQtText);
	if (!pm.isNull())
		box.setIconPixmap(pm);
	box.setDefaultButton(QMessageBox::Ok);

	box.frame()->setWindowIcon(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"));
	box.frame()->setWindowTitle(title.isEmpty() ? QString::fromLocal8Bit("����Qt") : title);
	box.titleBar()->setIcon(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"));
	box.titleBar()->setTitle(title.isEmpty() ? QString::fromLocal8Bit("����Qt") : title);

	box.frame()->show();
	box.frame()->raise();
	box.frame()->activateWindow();

	QMessageBox::StandardButton ret = (QMessageBox::StandardButton)box.exec();
	box.frame()->close();

	return ret;
}


QMessageBox::StandardButton QssMessageBox::information( QWidget *parent, const QString &title, const QString &text, StandardButtons buttons /*= Ok*/, StandardButton defaultButton /*= NoButton*/ )
{
	return QssMessageBox::tips(text, parent, title, buttons);
}

QMessageBox::StandardButton QssMessageBox::question( QWidget *parent, const QString &title, const QString &text, StandardButtons buttons /*= QMessageBox::Ok*/, StandardButton defaultButton /*= QMessageBox::NoButton*/ )
{
	return QssMessageBox::ask(text, parent, title, buttons);
}

QMessageBox::StandardButton QssMessageBox::warning( QWidget *parent, const QString &title, const QString &text, StandardButtons buttons /*= QMessageBox::Ok*/, StandardButton defaultButton /*= QMessageBox::NoButton*/ )
{
	return QssMessageBox::warn(text, parent, title, buttons);
}

QMessageBox::StandardButton QssMessageBox::critical( QWidget *parent, const QString &title, const QString &text, StandardButtons buttons /*= QMessageBox::Ok*/, StandardButton defaultButton /*= QMessageBox::NoButton*/ )
{
	return QssMessageBox::error(text, parent, title, buttons);
}

void QssMessageBox::about( QWidget *parent, const QString &title, const QString &text )
{
	QssMessageBox::regard(text, parent, title);
}

void QssMessageBox::about( QWidget *parent, const QString &title, const QString &text, QIcon icon )
{
	QssMessageBox::regard(text, icon, parent, title);
}

void QssMessageBox::aboutQt( QWidget *parent, const QString &title /*= QString()*/ )
{
	QssMessageBox::regardQt(parent, title);
}

