#include "testdialog.h"
#include "gui.h"
#include "main.h"

GUITestDialog::GUITestDialog() : GUIWindow("Test Dialog", 0, 0, 600, 600) {
  ListBox* listbox = new ListBox(20, 30, 200, 10);
  MemoBox* memobox = new MemoBox(240, 30, 200, 10);
  CheckBox* checkbox = new CheckBox(450, 30, 140);
  Buttons* buttons = new Buttons(450, 60);
  ProgressBar* progressbar = new ProgressBar(450, 90, 140, 10, 0, 100, 255, 100, 100);
  Object* object = new Object(450, 130, 2148, 55);
  ObjectSlot* objectslot = new ObjectSlot(500, 130, 2148, 23);

  for(int i = 0; i < 100; i++){
    std::stringstream ss;
    ss << "Item number " << i;
    
    listbox->addItem(ss.str());
  }
  
  listbox->select(55);
  
  memobox->setText("Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?");
  
  checkbox->setText("This is a CheckBox");
  
  buttons->pushButton("Button!", &GUI::nullDialog, NULL, 0, 0);
  
  progressbar->pos(34);
  
  GUIParent::addChild(listbox);
  GUIParent::addChild(memobox);
  GUIParent::addChild(checkbox);
  GUIParent::addChild(buttons);
  GUIParent::addChild(progressbar);
  GUIParent::addChild(object);
  GUIParent::addChild(objectslot);
  
  int inputXOffset = 20;
  int inputYOffset = 210;
  
  for(int y = 0; y < 10; y++){
    for(int x = 0; x < 3; x++){
      InputBox* inputbox = new InputBox(inputXOffset + x * 200, inputYOffset + y * 40, 150);
      Label* label = new Label(inputXOffset + x * 200, (inputYOffset - 12) + y * 40);
      
      int id = GUIParent::addChild(inputbox);
      
      std::stringstream text;
      text << "Id of: " << id;
      
      label->setText(text.str());
      GUIParent::addChild(label);
    }
  }
}

void GUITestDialog::tick(uint32_t ticks){

}
