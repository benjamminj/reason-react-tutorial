let component = ReasonReact.statelessComponent("App");

open Css;
open Json.Decode;

let make = _children => {
  ...component,
  render: _self =>
    <div> {ReasonReact.string("Hello world!")} </div>,
};