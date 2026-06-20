namespace ConfigTool
{
    partial class ConfigTool
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.LocationCombo = new System.Windows.Forms.ComboBox();
            this.ServiceTypeCombo = new System.Windows.Forms.ComboBox();
            this.ConfigViewer = new System.Windows.Forms.TextBox();
            this.UpdateBtn = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // LocationCombo
            // 
            this.LocationCombo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.LocationCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.LocationCombo.FormattingEnabled = true;
            this.LocationCombo.Location = new System.Drawing.Point(418, 12);
            this.LocationCombo.Name = "LocationCombo";
            this.LocationCombo.Size = new System.Drawing.Size(177, 20);
            this.LocationCombo.TabIndex = 0;
            this.LocationCombo.SelectedIndexChanged += new System.EventHandler(this.LocationCombo_SelectedIndexChanged);
            // 
            // ServiceTypeCombo
            // 
            this.ServiceTypeCombo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.ServiceTypeCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ServiceTypeCombo.FormattingEnabled = true;
            this.ServiceTypeCombo.Location = new System.Drawing.Point(241, 12);
            this.ServiceTypeCombo.Name = "ServiceTypeCombo";
            this.ServiceTypeCombo.Size = new System.Drawing.Size(142, 20);
            this.ServiceTypeCombo.TabIndex = 1;
            this.ServiceTypeCombo.SelectedIndexChanged += new System.EventHandler(this.ServiceTypeCombo_SelectedIndexChanged);
            // 
            // ConfigViewer
            // 
            this.ConfigViewer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ConfigViewer.Location = new System.Drawing.Point(12, 38);
            this.ConfigViewer.Multiline = true;
            this.ConfigViewer.Name = "ConfigViewer";
            this.ConfigViewer.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.ConfigViewer.Size = new System.Drawing.Size(723, 626);
            this.ConfigViewer.TabIndex = 2;
            // 
            // UpdateBtn
            // 
            this.UpdateBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.UpdateBtn.Location = new System.Drawing.Point(641, 9);
            this.UpdateBtn.Name = "UpdateBtn";
            this.UpdateBtn.Size = new System.Drawing.Size(94, 23);
            this.UpdateBtn.TabIndex = 4;
            this.UpdateBtn.Text = "갱신";
            this.UpdateBtn.UseVisualStyleBackColor = true;
            this.UpdateBtn.Click += new System.EventHandler(this.UpdateBtn_Click);
            // 
            // ConfigTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(751, 676);
            this.Controls.Add(this.UpdateBtn);
            this.Controls.Add(this.ConfigViewer);
            this.Controls.Add(this.ServiceTypeCombo);
            this.Controls.Add(this.LocationCombo);
            this.Name = "ConfigTool";
            this.Text = "Config Tool";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox LocationCombo;
        private System.Windows.Forms.ComboBox ServiceTypeCombo;
        private System.Windows.Forms.TextBox ConfigViewer;
        private System.Windows.Forms.Button UpdateBtn;
    }
}

