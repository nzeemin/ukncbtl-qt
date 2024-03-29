/*  This file is part of UKNCBTL.
    UKNCBTL is free software: you can redistribute it and/or modify it under the terms
of the GNU Lesser General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.
    UKNCBTL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License along with
UKNCBTL. If not, see <http://www.gnu.org/licenses/>. */

// WavPcmFile.cpp

#include "stdafx.h"
#include "WavPcmFile.h"
#include <stdio.h>


//////////////////////////////////////////////////////////////////////

// WAV PCM format description: https://ccrma.stanford.edu/courses/422/projects/WaveFormat/

static const char magic1[4] = { 'R', 'I', 'F', 'F' };
static const char magic2[4] = { 'W', 'A', 'V', 'E' };
static const char format_tag_id[4] = { 'f', 'm', 't', ' ' };
static const char data_tag_id[4] = { 'd', 'a', 't', 'a' };

const int WAV_FORMAT_PCM = 1;

struct WAVPCMFILE
{
    FILE* fpFile;
    int nChannels;
    int nBitsPerSample;
    int nSampleFrequency;
    int nBlockAlign;
    quint32 dwDataOffset;
    quint32 dwDataSize;
    quint32 dwCurrentPosition;
    bool okWriting;
};

int WavPcmFile_GetFrequency(HWAVPCMFILE wavpcmfile)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return 0;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;

    return pWavPcm->nSampleFrequency;
}

quint32 WavPcmFile_GetLength(HWAVPCMFILE wavpcmfile)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return 0;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;

    return pWavPcm->dwDataSize / pWavPcm->nBlockAlign;
}

quint32 WavPcmFile_GetPosition(HWAVPCMFILE wavpcmfile)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return 0;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;

    return pWavPcm->dwCurrentPosition;
}

void WavPcmFile_SetPosition(HWAVPCMFILE wavpcmfile, quint32 position)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;

    quint32 offsetInData = position * pWavPcm->nBlockAlign;
    ::fseek(pWavPcm->fpFile, pWavPcm->dwDataOffset + offsetInData, SEEK_SET);

    pWavPcm->dwCurrentPosition = position;
}

HWAVPCMFILE WavPcmFile_Create(LPCTSTR filename, int sampleRate)
{
    const int bitsPerSample = 8;
    const int channels = 1;
    const int blockAlign = channels * bitsPerSample / 8;

    FILE* fpFileNew = ::_tfopen(filename, _T("w+b"));
    if (fpFileNew == nullptr)
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to create file

    // Prepare and write file header
    quint8 consolidated_header[12 + 8 + 16 + 8];
    ::memset(consolidated_header, 0, sizeof(consolidated_header));
    size_t bytesWritten;

    memcpy(&consolidated_header[0], magic1, 4);  // RIFF
    memcpy(&consolidated_header[8], magic2, 4);  // WAVE

    memcpy(&consolidated_header[12], format_tag_id, 4);  // fmt
    *((quint32*)(consolidated_header + 16)) = 16;  // Size of "fmt" chunk
    *((quint16*)(consolidated_header + 20)) = WAV_FORMAT_PCM;  // AudioFormat = PCM
    *((quint16*)(consolidated_header + 22)) = channels;  // NumChannels = mono
    *((quint32*)(consolidated_header + 24)) = sampleRate;  // SampleRate
    *((quint32*)(consolidated_header + 28)) = sampleRate * channels * bitsPerSample / 8;  // ByteRate
    *((quint16*)(consolidated_header + 32)) = blockAlign;
    *((quint16*)(consolidated_header + 34)) = bitsPerSample;

    memcpy(&consolidated_header[36], data_tag_id, 4);  // data

    // Write consolidated header
    bytesWritten = ::fwrite(consolidated_header, 1, sizeof(consolidated_header), fpFileNew);
    if (bytesWritten != sizeof(consolidated_header))
    {
        ::fclose(fpFileNew);
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to write consolidated header
    }

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) ::malloc(sizeof(WAVPCMFILE));
    if (pWavPcm == nullptr)
    {
        ::fclose(fpFileNew);
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to allocate memory
    }
    memset(pWavPcm, 0, sizeof(WAVPCMFILE));
    pWavPcm->fpFile = fpFileNew;
    pWavPcm->nChannels = channels;
    pWavPcm->nSampleFrequency = sampleRate;
    pWavPcm->nBitsPerSample = bitsPerSample;
    pWavPcm->nBlockAlign = blockAlign;
    pWavPcm->dwDataOffset = sizeof(consolidated_header);
    pWavPcm->dwDataSize = 0;
    pWavPcm->okWriting = true;

    WavPcmFile_SetPosition((HWAVPCMFILE) pWavPcm, 0);

    return (HWAVPCMFILE) pWavPcm;
}

HWAVPCMFILE WavPcmFile_Open(LPCTSTR filename)
{
    FILE* fpFileOpen = ::_tfopen(filename, _T("rb"));
    if (fpFileOpen == nullptr)
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to open file

    quint32 offset = 0;
    size_t bytesRead;
    ::fseek(fpFileOpen, 0, SEEK_END);
    quint32 fileSize = ::ftell(fpFileOpen);
    ::fseek(fpFileOpen, 0, SEEK_SET);

    quint8 fileHeader[12];
    bytesRead = ::fread(fileHeader, 1, sizeof(fileHeader), fpFileOpen);
    if (bytesRead != sizeof(fileHeader) ||
        memcmp(&fileHeader[0], magic1, 4) != 0 ||
        memcmp(&fileHeader[8], magic2, 4) != 0)
    {
        ::fclose(fpFileOpen);
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to read file header OR invalid 'RIFF' tag OR invalid 'WAVE' tag
    }
    offset += (quint32)bytesRead;

    quint32 statedSize = *((quint32*)(fileHeader + 4)) + 8;
    if (statedSize > fileSize)
        statedSize = fileSize;

    quint8 tagHeader[8];
    quint16 formatTag[8];
    bool formatSpecified = false;
    int formatType, channels = 1, bitsPerSample, blockAlign;
    quint32 sampleFrequency, bytesPerSecond, dataOffset, dataSize = 0;
    while (offset < statedSize)
    {
        bytesRead = ::fread(tagHeader, 1, sizeof(tagHeader), fpFileOpen);
        if (bytesRead != sizeof(tagHeader))
        {
            ::fclose(fpFileOpen);
            return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to read tag header
        }
        offset += (quint32)bytesRead;

        quint32 tagSize = *(quint32*)(tagHeader + 4);
        if (!memcmp(tagHeader, format_tag_id, 4))
        {
            if (formatSpecified || tagSize < sizeof(formatTag))
            {
                ::fclose(fpFileOpen);
                return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Wrong tag header
            }
            formatSpecified = true;

            bytesRead = ::fread(formatTag, 1, sizeof(formatTag), fpFileOpen);
            if (bytesRead != sizeof(formatTag))
            {
                ::fclose(fpFileOpen);
                return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to read format tag
            }

            formatType = formatTag[0];
            channels = formatTag[1];
            sampleFrequency = formatTag[2];
            bytesPerSecond = formatTag[4];
            blockAlign = formatTag[6];
            bitsPerSample = formatTag[7];

            if (formatType != WAV_FORMAT_PCM)
            {
                ::fclose(fpFileOpen);
                return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Unsupported format
            }
            if (sampleFrequency * bitsPerSample * channels / 8 != bytesPerSecond ||
                (bitsPerSample != 8 && bitsPerSample != 16 && bitsPerSample != 32))
            {
                ::fclose(fpFileOpen);
                return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Wrong format tag
            }
        }
        else if (!memcmp(tagHeader, data_tag_id, 4))
        {
            if (!formatSpecified)
            {
                ::fclose(fpFileOpen);
                return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Wrong tag
            }

            dataOffset = offset;
            dataSize = tagSize;
        }
        else  // Ignore all other tags
        {
        }

        offset += tagSize;
        ::fseek(fpFileOpen, offset, SEEK_SET);
    }

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) ::malloc(sizeof(WAVPCMFILE));
    if (pWavPcm == nullptr)
    {
        ::fclose(fpFileOpen);
        return (HWAVPCMFILE) INVALID_HANDLE_VALUE;  // Failed to allocate memory
    }
    ::memset(pWavPcm, 0, sizeof(WAVPCMFILE));
    pWavPcm->fpFile = fpFileOpen;
    pWavPcm->nChannels = channels;
    pWavPcm->nSampleFrequency = sampleFrequency;
    pWavPcm->nBitsPerSample = bitsPerSample;
    pWavPcm->nBlockAlign = blockAlign;
    pWavPcm->dwDataOffset = dataOffset;
    pWavPcm->dwDataSize = dataSize;
    pWavPcm->okWriting = false;

    WavPcmFile_SetPosition((HWAVPCMFILE) pWavPcm, 0);

    return (HWAVPCMFILE) pWavPcm;
}

void WavPcmFile_Close(HWAVPCMFILE wavpcmfile)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;

    if (pWavPcm->okWriting)
    {
        size_t bytesWritten;
        // Write data chunk size
        ::fseek(pWavPcm->fpFile, 4, SEEK_SET);
        quint32 chunkSize = 36 + pWavPcm->dwDataSize;
        bytesWritten = ::fwrite(&chunkSize, 1, 4, pWavPcm->fpFile);
        // Write data subchunk size
        ::fseek(pWavPcm->fpFile, 40, SEEK_SET);
        bytesWritten = ::fwrite(&(pWavPcm->dwDataSize), 1, 4, pWavPcm->fpFile);
    }

    ::fclose(pWavPcm->fpFile);
    pWavPcm->fpFile = nullptr;
    ::free(pWavPcm);
}

bool WavPcmFile_WriteOne(HWAVPCMFILE wavpcmfile, unsigned int value)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return false;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;
    if (!pWavPcm->okWriting)
        return false;
    ASSERT(pWavPcm->nBitsPerSample == 8);
    ASSERT(pWavPcm->nChannels == 1);

    quint8 data = (value >> 24) & 0xff;

    size_t bytesWritten = ::fwrite(&data, 1, 1, pWavPcm->fpFile);
    if (bytesWritten != 1)
        return false;

    pWavPcm->dwCurrentPosition++;
    pWavPcm->dwDataSize += pWavPcm->nBlockAlign;

    return true;
}

unsigned int WavPcmFile_ReadOne(HWAVPCMFILE wavpcmfile)
{
    if (wavpcmfile == INVALID_HANDLE_VALUE)
        return 0;

    WAVPCMFILE* pWavPcm = (WAVPCMFILE*) wavpcmfile;
    if (pWavPcm->okWriting)
        return 0;

    // Read one sample
    quint32 bytesToRead = pWavPcm->nBlockAlign;
    size_t bytesRead;
    quint8 data[16];
    bytesRead = ::fread(data, 1, bytesToRead, pWavPcm->fpFile);
    if (bytesRead != bytesToRead)
        return 0;

    pWavPcm->dwCurrentPosition++;

    // Decode first channel
    unsigned int value = 0;
    switch (pWavPcm->nBitsPerSample)
    {
    case 8:
        value = *data;
        value = value << 24;
        break;
    case 16:
        value = *((quint16*)data);
        value = value << 16;
        break;
    case 32:
        value = *((quint32*)data);
        break;
    }

    return value;
}


//////////////////////////////////////////////////////////////////////
