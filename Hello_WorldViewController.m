//
//  Hello_WorldViewController.m
//  Hello World
//
//  Created by Alexander Nathan on 3/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Hello_WorldViewController.h"

@interface Hello_WorldViewController ()

@end

@implementation Hello_WorldViewController

@synthesize userName = _userName;

@synthesize label;
@synthesize textField;

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [self setTextField:nil];
    [self setLabel:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (IBAction)changeGreeting:(id)sender {
    
    self.userName = self.textField.text; //takes text from textfield and sets the view controller's userName property(variable) to it
    
    NSString *nameString = self.userName; //creates a new variable and sets it to the view controller's userName property(variable)
    
    if([nameString length]==0){
        nameString=@"World"; 
    }
    
    NSString *greeting =[[NSString alloc] initWithFormat:@"Hello, %@!",nameString];//initWithFormat creates a new string that follows the format specified by @"Hello, %@!"
    
    self.label.text=greeting;
}

-(BOOL)textFieldShouldReturn:(UITextField *)theTextField {
    if(theTextField == self.textField) {
        [theTextField resignFirstResponder]; 
    }
    
    return YES;
}
@end
