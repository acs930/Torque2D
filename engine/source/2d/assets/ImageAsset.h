//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _IMAGE_ASSET_H_
#define _IMAGE_ASSET_H_

#ifndef _ASSET_BASE_H_
#include "assets/assetBase.h"
#endif

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType( TypeImageAssetPtr )

//-----------------------------------------------------------------------------

class ImageAsset : public AssetBase
{
private:
    typedef AssetBase Parent;

public:
    /// Texture Filter Mode.
    enum TextureFilterMode
    {
        FILTER_NEAREST,
        FILTER_BILINEAR,

        FILTER_INVALID,
    };

    /// Frame area.
    class FrameArea
    {
    public:
        /// Frame Pixel Area.
        class PixelArea
        {
        public:
            PixelArea() {}
            PixelArea( const S32 pixelFrameOffsetX, const S32 pixelFrameOffsetY, const U32 pixelFrameWidth, const U32 pixelFrameHeight )
            {
                setArea( pixelFrameOffsetX, pixelFrameOffsetY, pixelFrameWidth, pixelFrameHeight );
            }

            inline void setArea( const S32 pixelFrameOffsetX, const S32 pixelFrameOffsetY, const U32 pixelFrameWidth, const U32 pixelFrameHeight )
            {
                mPixelOffset.set( pixelFrameOffsetX, pixelFrameOffsetY );
                mPixelWidth = pixelFrameWidth;
                mPixelHeight = pixelFrameHeight;
            };

            Point2I mPixelOffset;
            U32 mPixelWidth;
            U32 mPixelHeight;
        };


        /// Frame Texel Area.
        class TexelArea
        {
        public:
            TexelArea() {}
            TexelArea( const PixelArea& pixelArea, const F32 texelWidthScale, const F32 texelHeightScale )
            {
                setArea( pixelArea, texelWidthScale, texelHeightScale );
            }

            void setArea( const PixelArea& pixelArea, const F32 texelWidthScale, const F32 texelHeightScale )
            {
                mTexelLower.Set( pixelArea.mPixelOffset.x * texelWidthScale, pixelArea.mPixelOffset.y * texelHeightScale );
                mTexelWidth = pixelArea.mPixelWidth * texelWidthScale;
                mTexelHeight = pixelArea.mPixelHeight * texelHeightScale;
                mTexelUpper.Set( mTexelLower.x + mTexelWidth, mTexelLower.y + mTexelHeight );
            }

            void setFlip( const bool flipX, const bool flipY )
            {
                if ( flipX ) mSwap( mTexelLower.x, mTexelUpper.x );
                if ( flipY ) mSwap( mTexelLower.y, mTexelUpper.y );
            }

            Vector2 mTexelLower;
            Vector2 mTexelUpper;
            F32 mTexelWidth;
            F32 mTexelHeight;
        };

    public:
        FrameArea( const S32 pixelFrameOffsetX, const S32 pixelFrameOffsetY, const U32 pixelFrameWidth, const U32 pixelFrameHeight, const F32 texelWidthScale, const F32 texelHeightScale )
        {
            setArea( pixelFrameOffsetX, pixelFrameOffsetY, pixelFrameWidth, pixelFrameHeight, texelWidthScale, texelHeightScale );
        }

        void setArea( const S32 pixelFrameOffsetX, const S32 pixelFrameOffsetY, const U32 pixelFrameWidth, const U32 pixelFrameHeight, const F32 texelWidthScale, const F32 texelHeightScale )
        {
            mPixelArea.setArea( pixelFrameOffsetX, pixelFrameOffsetY, pixelFrameWidth, pixelFrameHeight );
            mTexelArea.setArea( mPixelArea, texelWidthScale, texelHeightScale );
        }

        PixelArea mPixelArea;
        TexelArea mTexelArea;
    };

private:
    typedef Vector<FrameArea> typeFrameAreaVector;
    typedef Vector<FrameArea::PixelArea> typeExplicitFrameAreaVector;
    typedef HashMap<StringTableEntry, FrameArea::PixelArea> typeNameFrameAreaHash;

    /// Configuration.
    StringTableEntry            mImageFile;
    bool						mForce16Bit;
    TextureFilterMode           mLocalFilterMode;
    bool                        mExplicitMode;
    bool                        mNameMode;
    bool                        mCellRowOrder;
    S32                         mCellOffsetX;
    S32                         mCellOffsetY;
    S32                         mCellStrideX;
    S32                         mCellStrideY;
    S32                         mCellWidth;
    S32                         mCellHeight;
    S32                         mCellCountX;
    S32                         mCellCountY;

    /// Imagery.
    typeFrameAreaVector         mFrames;
    typeExplicitFrameAreaVector mExplicitFrames;
    typeNameFrameAreaHash       mNamedFrames;
    TextureHandle               mImageTextureHandle;

public:
    ImageAsset();
    virtual ~ImageAsset();

    /// Core.
    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    void                    setImageFile( const char* pImageFile );
    inline StringTableEntry getImageFile( void ) const                      { return mImageFile; };

    void                    setForce16Bit( const bool force16Bit );
    inline bool             getForce16Bit( void ) const                     { return mForce16Bit; }

    void                    setFilterMode( const TextureFilterMode filterMode );
    TextureFilterMode       getFilterMode( void ) const                     { return mLocalFilterMode; }

    void                    setExplicitMode( const bool explicitMode );
    bool                    getExplicitMode( void ) const                   { return mExplicitMode; }

    void                    setNameMode( const bool nameMode );
    bool                    getNameMode( void ) const                       {return mNameMode; }

    void                    setCellRowOrder( const bool cellRowOrder );
    inline bool             getCellRowOrder( void ) const                   { return mCellRowOrder; }

    void                    setCellOffsetX( const S32 cellOffsetX );
    inline S32              getCellOffsetX( void ) const                    { return mCellOffsetX; }

    void                    setCellOffsetY( const S32 cellOffsetY );
    inline S32              getCellOffsetY( void ) const                    { return mCellOffsetY; }

    void                    setCellStrideX( const S32 cellStrideX );
    inline S32              getCellStrideX( void ) const                    { return mCellStrideX; }

    void                    setCellStrideY( const S32 cellStrideY );
    inline S32              getCellStrideY( void ) const                    { return mCellStrideY; }

    void                    setCellCountX( const S32 cellCountX );
    inline S32              getCellCountX( void ) const                     { return mCellCountX; }

    void                    setCellCountY( const S32 cellCountY );
    inline S32              getCellCountY( void ) const                     { return mCellCountY; }

    void                    setCellWidth( const S32 cellWidth );
    inline S32              getCellWidth( void ) const						{ return mCellWidth; }

    void                    setCellHeight( const S32 cellheight );
    S32                     getCellHeight( void) const						{ return mCellHeight; }

    inline TextureHandle&   getImageTexture( void )                         { return mImageTextureHandle; }
    inline S32              getImageWidth( void ) const                     { return mImageTextureHandle.getWidth(); }
    inline S32              getImageHeight( void ) const                    { return mImageTextureHandle.getHeight(); }
    inline U32              getFrameCount( void ) const                     { return (U32)mFrames.size(); };
    inline bool             containsFrame( const char* namedFrame ) const                     { return false; };

    inline const FrameArea& getImageFrameArea( U32 frame ) const            { clampFrame(frame); return mFrames[frame]; };
    inline const FrameArea& getImageFrameArea( const char* namedFrame) const    { return mFrames[0]; };
    inline const void       bindImageTexture( void)                         { glBindTexture( GL_TEXTURE_2D, getImageTexture().getGLName() ); };
    
    virtual bool            isAssetValid( void ) const                      { return !mImageTextureHandle.IsNull(); }

    /// Explicit cell control.
    bool                    clearExplicitCells( void );
    bool                    addExplicitCell( const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    bool                    insertExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    bool                    removeExplicitCell( const S32 cellIndex );
    bool                    setExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    inline S32              getExplicitCellCount( void ) const              { return mExplicitFrames.size(); }
    
    /// Named cell control.
    bool                    clearNamedCells( void );
    bool                    addNamedCell( const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    bool                    insertNamedCell( const char* cellName, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    bool                    removeNamedCell( const char* cellName );
    bool                    setNamedCell( const char* cellName, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight );
    inline S32              getNamedCellCount( void ) const              { return mNamedFrames.size(); }

    static TextureFilterMode getFilterModeEnum(const char* label);
    static const char* getFilterModeDescription( TextureFilterMode filterMode );

    /// Declare Console Object.
    DECLARE_CONOBJECT(ImageAsset);

private:
    inline void clampFrame( U32& frame ) const                              { const U32 totalFrames = getFrameCount(); if ( frame >= totalFrames ) frame = (totalFrames == 0 ? 0 : totalFrames-1 ); };
    void calculateImage( void );
    void calculateImplicitMode( void );
    void calculateExplicitMode( void );
    void calculateNameMode( void );
    void setTextureFilter( const TextureFilterMode filterMode );

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

    /// Taml callbacks.
    virtual void onTamlPreWrite( void );
    virtual void onTamlPostWrite( void );
    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes );


protected:
    static void textureEventCallback( const U32 eventCode, void *userData );

    static bool setImageFile( void* obj, const char* data )                 { static_cast<ImageAsset*>(obj)->setImageFile(data); return false; }
    static const char* getImageFile(void* obj, const char* data)            { return static_cast<ImageAsset*>(obj)->getImageFile(); }
    static bool writeImageFile( void* obj, StringTableEntry pFieldName )    { return static_cast<ImageAsset*>(obj)->getImageFile() != StringTable->EmptyString; }

    static bool setForce16Bit( void* obj, const char* data )                { static_cast<ImageAsset*>(obj)->setForce16Bit(dAtob(data)); return false; }
    static bool writeForce16Bit( void* obj, StringTableEntry pFieldName )   { return static_cast<ImageAsset*>(obj)->getForce16Bit() == true; }

    static bool setFilterMode( void* obj, const char* data );
    static bool writeFilterMode( void* obj, StringTableEntry pFieldName )   { return static_cast<ImageAsset*>(obj)->getFilterMode() != FILTER_BILINEAR; }

    static bool setExplicitMode( void* obj, const char* data )              { static_cast<ImageAsset*>(obj)->setExplicitMode(dAtob(data)); return false; }
    static bool setNameMode( void* obj, const char* data )                  { static_cast<ImageAsset*>(obj)->setNameMode(dAtob(data)); return false; }

    static bool setCellRowOrder( void* obj, const char* data )              { static_cast<ImageAsset*>(obj)->setCellRowOrder(dAtob(data)); return false; }
    static bool writeCellRowOrder( void* obj, StringTableEntry pFieldName ) { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && !pImageAsset->getCellRowOrder(); }

    static bool setCellOffsetX( void* obj, const char* data )               { static_cast<ImageAsset*>(obj)->setCellOffsetX(dAtoi(data)); return false; }
    static bool writeCellOffsetX( void* obj, StringTableEntry pFieldName )  { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellOffsetX() != 0; }

    static bool setCellOffsetY( void* obj, const char* data )               { static_cast<ImageAsset*>(obj)->setCellOffsetY(dAtoi(data)); return false; }
    static bool writeCellOffsetY( void* obj, StringTableEntry pFieldName )  { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellOffsetY() != 0; }

    static bool setCellStrideX( void* obj, const char* data )               { static_cast<ImageAsset*>(obj)->setCellStrideX(dAtoi(data)); return false; }
    static bool writeCellStrideX( void* obj, StringTableEntry pFieldName )  { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellStrideX() != 0; }

    static bool setCellStrideY( void* obj, const char* data )               { static_cast<ImageAsset*>(obj)->setCellStrideY(dAtoi(data)); return false; }
    static bool writeCellStrideY( void* obj, StringTableEntry pFieldName )  { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellStrideY() != 0; }

    static bool setCellCountX( void* obj, const char* data )                { static_cast<ImageAsset*>(obj)->setCellCountX(dAtoi(data)); return false; }
    static bool writeCellCountX( void* obj, StringTableEntry pFieldName )   { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellCountX() != 0; }

    static bool setCellCountY( void* obj, const char* data )                { static_cast<ImageAsset*>(obj)->setCellCountY(dAtoi(data)); return false; }
    static bool writeCellCountY( void* obj, StringTableEntry pFieldName )   { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellCountY() != 0; }

    static bool setCellWidth( void* obj, const char* data )                 { static_cast<ImageAsset*>(obj)->setCellWidth(dAtoi(data)); return false; }
    static bool writeCellWidth( void* obj, StringTableEntry pFieldName )    { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellWidth() != 0; }

    static bool setCellHeight( void* obj, const char* data )                { static_cast<ImageAsset*>(obj)->setCellHeight(dAtoi(data)); return false; }
    static bool writeCellHeight( void* obj, StringTableEntry pFieldName )   { ImageAsset* pImageAsset = static_cast<ImageAsset*>(obj); return !pImageAsset->getExplicitMode() && !pImageAsset->getNameMode() && pImageAsset->getCellHeight() != 0; }
};

//-----------------------------------------------------------------------------

extern ImageAsset::FrameArea BadFrameArea;

#endif // _IMAGE_ASSET_H_
