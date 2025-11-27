AudioSignalResClass {
 Inputs {
  IOPItemInputClass {
   id 1
   name "Max"
   tl -200 0
   children {
    4
   }
  }
  IOPInputRandomClass {
   id 3
   name "Random"
   tl -202 -95
   children {
    4
   }
  }
 }
 Ops {
  IOPItemOpMulClass {
   id 4
   name "Mul"
   tl 14 -3
   children {
    2
   }
   inputs {
    ConnectionClass "1:0" {
     id 1
     port 0
    }
    ConnectionClass "3:0" {
     id 3
     port 0
    }
   }
  }
 }
 Outputs {
  IOPItemOutputClass {
   id 2
   name "Value"
   tl 203 0
   input 4
  }
 }
 Input_Order {
  ItemDetailListItemClass Max {
   Name "Max"
   Id 1
  }
 }
 Output_Order {
  ItemDetailListItemClass Value {
   Name "Value"
   Id 2
  }
 }
}