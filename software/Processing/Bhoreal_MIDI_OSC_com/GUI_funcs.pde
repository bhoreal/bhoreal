
public void cpPad0() {
  note.setChannel(1);
  note.setPitch(0);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad1() {
  note.setChannel(1);
  note.setPitch(1);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad2() {
  note.setChannel(1);
  note.setPitch(2);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad3() {
  note.setChannel(1);
  note.setPitch(3);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad4() {
  note.setChannel(1);
  note.setPitch(4);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad5() {
  note.setChannel(1);
  note.setPitch(5);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad6() {
  note.setChannel(1);
  note.setPitch(6);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad7() {
  note.setChannel(1);
  note.setPitch(7);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad8() {
  note.setChannel(1);
  note.setPitch(8);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad9() {
  note.setChannel(1);
  note.setPitch(9);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad10() {
  note.setChannel(1);
  note.setPitch(10);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad11() {
  note.setChannel(1);
  note.setPitch(11);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad12() {
  note.setChannel(1);
  note.setPitch(12);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad13() {
  note.setChannel(1);
  note.setPitch(13);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad14() {
  note.setChannel(1);
  note.setPitch(14);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}
public void cpPad15() {
  note.setChannel(1);
  note.setPitch(15);
  note.setVelocity(PAD_Color);
  myBus.sendNoteOn(note);
}

int[] fromPadtoMatrix(int pad){ 
 int[] padXY = new int[2];
 
 switch(pad) {
  case 0: 
    padXY[0] = 0;
    padXY[1] = 0;
    break;
  case 1: 
    padXY[0] = 1;
    padXY[1] = 0;
    break;
  case 2: 
    padXY[0] = 2;
    padXY[1] = 0;
    break;
  case 3: 
    padXY[0] = 3;
    padXY[1] = 0;
    break;
  case 4: 
    padXY[0] = 0;
    padXY[1] = 1;
    break;
  case 5: 
    padXY[0] = 1;
    padXY[1] = 1;
    break;
  case 6: 
    padXY[0] = 2;
    padXY[1] = 1;
    break;
  case 7: 
    padXY[0] = 3;
    padXY[1] = 1;
    break;
  case 8: 
    padXY[0] = 0;
    padXY[1] = 2;
    break;
  case 9: 
    padXY[0] = 1;
    padXY[1] = 2;
    break;
  case 10: 
    padXY[0] = 2;
    padXY[1] = 2;
    break;
  case 11: 
    padXY[0] = 0;
    padXY[1] = 3;
    break;
  case 12: 
    padXY[0] = 0;
    padXY[1] = 3;
    break;
  case 13: 
    padXY[0] = 1;
    padXY[1] = 3;
    break;
  case 14: 
    padXY[0] = 2;
    padXY[1] = 3;
    break;
  case 15: 
    padXY[0] = 3;
    padXY[1] = 3;
    break;
  }
  return padXY;
}