#include "qsoundout.h"
#include "QAudioFormat"
QSoundOut::QSoundOut(QObject *parent) :
    QObject(parent)
{
    rcnt=0;
    rrd=0;
    rwr=0;
    fptr=0;
    m_audio=NULL;
    m_dev=NULL;

    QAudioFormat fmt;
    fmt.setFrequency(SAMPLERATE);
    fmt.setChannels(CHANNELS);
    fmt.setSampleSize(SAMPLESIZE);
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setCodec("audio/pcm");
    fmt.setSampleType(QAudioFormat::SignedInt);

    m_audio=new QAudioOutput(fmt,this);
    if(m_audio==NULL)
        return;
    connect(m_audio,SIGNAL(notify()),this,SLOT(OnNotify()));
    m_audio->setBufferSize(FRAMEBYTES*2);
    m_audio->setNotifyInterval(40); //1/25 sec = 40mS
    m_dev=m_audio->start();
    if(m_dev==NULL)
    {
        disconnect(this,SLOT(OnNotify()));
        delete(m_audio);
        m_audio=NULL;
        return;
    }

    m_kick.setSingleShot(true);
    m_kick.connect(&m_kick,SIGNAL(timeout()),this,SLOT(OnNotify()));
    m_kick.start(40); //kick it first!

}

QSoundOut::~QSoundOut()
{
    if(m_dev)
    {
        m_audio->stop();
        disconnect(this,SLOT(OnNotify()));
        delete(m_audio); //this will delete m_dev too!!!!
        m_audio=NULL;
    }
}

void QSoundOut::OnNotify()
{
    char dummy[FRAMEBYTES];

    if((m_dev==NULL)||(m_audio==NULL))
        return;

    m_lock.lock();
    if(rcnt)
    {
        m_dev->write((const char*)&rbuf[rrd],(qint64)FRAMEBYTES);
        rrd++;
        if(rrd>=BUFFERS)
            rrd=0;
        rcnt--;
    }
    else
    {
        memset(dummy,0,FRAMEBYTES);
        m_dev->write((const char*)dummy,(qint64)FRAMEBYTES);
    }
    m_lock.unlock();
}

void QSoundOut::FeedDAC(unsigned short left, unsigned short right)
{
    switch(SAMPLESIZE)
    {
        case 8:
            fbuf[fptr++]=left>>8;
            fbuf[fptr++]=right>>8;
        break;
        case 16:
            fbuf[fptr++]=left&0xff;
            fbuf[fptr++]=left>>8;
            fbuf[fptr++]=right&0xff;
            fbuf[fptr++]=right>>8;
        break;
    }
    if(fptr>=FRAMEBYTES)
    {
        fptr=0;

        while(rcnt>=BUFFERS); //wait untill there are some free buffers... Mutex in callback will prevent schedule

        m_lock.lock();
            memcpy(&rbuf[rwr],fbuf,FRAMEBYTES);
            rwr++;
            if(rwr>=BUFFERS)
                rwr=0;
            rcnt++;
        m_lock.unlock();
    }
}
