//
//  xxiivvBifView.h
//  Bifurcan
//
//  Created by Patrick Winchell on 7/13/14.
//  Copyright (c) 2014 XXIIVV. All rights reserved.
//


#if TARGET_OS_IPHONE
#define imageType UIImage
#define rectType  CGRect
#define colorType  UIColor
#define viewType  UIView
#else
#define imageType NSImage
#define rectType  NSRect
#define colorType  NSColor
#define viewType   NSView
#endif


#import <AVFoundation/AVFoundation.h>

#if TARGET_OS_IPHONE

#import <UIKit/UIKit.h>

#else

#import <Cocoa/Cocoa.h>

#endif


@interface xxiivvBifView : viewType
{
    imageType *srcleft;
    imageType *srcright;
    
    imageType *left;
    imageType *right;
    
    float lastGridSize;
    
    AVAudioPlayer *audioPlayer;
    AVAudioPlayer *audioPlayerAmbience;
    
    BOOL doFlash;
    
    NSTimer *timer;
    bool soundEnabled;
    int modeCurrent;
}
-(void)setFilterType:(int)filterType;
-(void)setSound:(bool)shouldSound;

@end



